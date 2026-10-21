/**
 * Copyright (c) 2024-2026 Alexandr Svetlichnyi, Savva Savenkov, Artemii Novikov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "EventData.hh"

#include <COLA.hh>
#include <gtest/gtest.h>

#include <cstdint>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace cola;

namespace {

  template <typename T, typename... Args>
  void AppendMakeVector(std::vector<T>& results, T&& arg, Args&&... args) {
    results.emplace_back(std::forward<T>(arg));
    if constexpr (sizeof...(args) > 0) {
      AppendMakeVector(results, std::forward<Args>(args)...);
    }
  }

  template <typename T, typename... Args>
  std::vector<T> MakeVector(T&& arg, Args&&... args) {
    std::vector<T> results;

    AppendMakeVector(results, std::forward<T>(arg), std::forward<Args>(args)...);

    return results;
  }

}  // namespace

class TestGenerator : public VGenerator {
 public:
  static inline const std::string kName = "TestGenerator";

  int call_counter = 0;
  EventData reference_data = GetDefaultEventData();

  std::unique_ptr<EventData> operator()() override {
    ++call_counter;

    return std::make_unique<EventData>(reference_data);
  }

 private:
  static EventData GetDefaultEventData() {
    EventData event;
    event.ini_state.pdg_code_a = 1000010020;
    event.ini_state.pdg_code_b = 1000010020;
    event.ini_state.pz_a = 1.0;
    event.ini_state.pz_b = 0.0;
    event.ini_state.energy = 10.0;

    Particle p1;
    p1.pdg_code = cola::AZToPdg({1, 0});
    p1.momentum = LorentzVector{.e = 1.0, .x = 0.1, .y = 0.2, .z = 0.3};
    p1.position = LorentzVector{.e = 0.0, .x = 0.0, .y = 0.0, .z = 0.0};
    p1.p_class = ParticleClass::kProduced;
    event.particles.push_back(p1);

    return event;
  }
};

class TestConverter : public VConverter {
 public:
  static inline const std::string kName = "TestConverter";

  int call_counter = 0;

  std::unique_ptr<EventData> operator()(std::unique_ptr<EventData>&& data) override {
    ++call_counter;

    for (auto& particle : data->particles) {
      particle.momentum.x *= 2.0;
    }
    return data;
  }
};

class TestWriter : public VWriter {
 public:
  static inline const std::string kName = "TestWriter";

  int call_counter = 0;
  std::vector<std::unique_ptr<EventData>> recorded_events;

  void operator()(std::unique_ptr<EventData>&& data) override {
    ++call_counter;
    recorded_events.emplace_back(std::move(data));
  }
};

namespace {

  class ConfiguredConverter : public VConverter {
   public:
    // NOLINTNEXTLINE(bugprone-throwing-static-initialization)
    static inline const std::string kName = "ConfiguredConverter";

    explicit ConfiguredConverter(const std::unordered_map<std::string, std::string>& parameters)
        : factor_(std::stod(parameters.at("factor"))) {}

    std::unique_ptr<EventData> operator()(std::unique_ptr<EventData>&& data) override {
      for (auto& particle : data->particles) {
        particle.momentum.x *= factor_;
      }
      return std::move(data);
    }

   private:
    double factor_;
  };

  class OptionalConfiguredConverter : public ConfiguredConverter {
   public:
    OptionalConfiguredConverter() : ConfiguredConverter({{"factor", "1"}}) {}
    using ConfiguredConverter::ConfiguredConverter;
  };

}  // namespace

TEST(ColaFactory, MetadataConstructorPreferredOverDefault) {
  GenericFactory<OptionalConfiguredConverter> factory;
  auto filter = factory.Create({{"factor", "3"}});
  auto* converter = dynamic_cast<VConverter*>(filter.get());
  ASSERT_NE(converter, nullptr);
  TestGenerator generator;
  const auto result = (*converter)(generator());
  EXPECT_DOUBLE_EQ(result->particles[0].momentum.x, 0.1 * 3);
}

TEST(ColaFactory, InvalidMetadataPropagates) {
  GenericFactory<ConfiguredConverter> factory;
  EXPECT_THROW(factory.Create({}), std::out_of_range);
  EXPECT_THROW(factory.Create({{"factor", "invalid"}}), std::invalid_argument);
}

TEST(ColaFactory, XmlConfiguresNonDefaultConstructibleFilter) {
  auto xml = std::stringstream(R"(<root>
      <generator name="TestGenerator"/>
      <converter name="ConfiguredConverter" factor="4"/>
      <writer name="TestWriter"/>
    </root>)");
  MetaProcessor processor;
  processor.Register(std::make_unique<GenericFactory<TestGenerator>>());
  processor.Register(std::make_unique<GenericFactory<ConfiguredConverter>>());
  processor.Register(std::make_unique<GenericFactory<TestWriter>>());
  auto ensemble = processor.Parse(xml);
  auto* writer = dynamic_cast<TestWriter*>(ensemble.writer.get());
  ASSERT_NE(writer, nullptr);
  ColaRunManager manager(std::move(ensemble));
  manager.Run();
  ASSERT_EQ(writer->recorded_events.size(), 1);
  EXPECT_DOUBLE_EQ(writer->recorded_events[0]->particles[0].momentum.x, 0.4);
}

TEST(ColaPdgTest, CircularConsistency) {
  for (uint16_t atomic_mass = 1; atomic_mass < 100; ++atomic_mass) {
    for (uint16_t charge_number = 0; charge_number <= atomic_mass; ++charge_number) {
      auto pdg_code = cola::AZToPdg({atomic_mass, charge_number});
      auto [circularA, circularZ] = cola::PdgToAZ(pdg_code);
      EXPECT_EQ(circularA, atomic_mass) << "Failed conversion for A = " << atomic_mass << ", Z = " << charge_number;
      EXPECT_EQ(circularZ, charge_number) << "Failed conversion for A = " << atomic_mass << ", Z = " << charge_number;
    }
  }
}

TEST(ColaPdgTest, PDGToAZForNeutron) {
  auto [a, z] = PdgToAZ(2112);
  EXPECT_EQ(a, 1);
  EXPECT_EQ(z, 0);
}

TEST(ColaPdgTest, PDGToAZForProton) {
  auto [a, z] = PdgToAZ(2212);
  EXPECT_EQ(a, 1);
  EXPECT_EQ(z, 1);
}

TEST(ColaPdgTest, AZToPDGForNeutron) {
  auto az = AZ{1, 0};
  auto pdg_code = AZToPdg(az);
  EXPECT_EQ(pdg_code, 2112);
}

TEST(ColaParse, InvalidFilePath) {
  MetaProcessor processor;

  EXPECT_THROW(processor.Parse("nonexistent/file.xml"), std::runtime_error);
}

TEST(ColaParse, BadXMLContents) {
  auto bad_xml_stream = std::stringstream(R"(<?xml version="1.0"?><no end block>)");

  MetaProcessor processor;

  EXPECT_THROW(processor.Parse(bad_xml_stream), std::runtime_error);
}

TEST(ColaParse, NoGenerator) {
  auto xml_stream = std::stringstream(R"(<?xml version="1.0"?>
    <root>
        <converter name="TestConverter"/>
        <writer name="TestWriter"/>
    </root>
    )");

  MetaProcessor processor;
  processor.Register(std::make_unique<GenericFactory<TestGenerator>>(), "TestGenerator");
  processor.Register(std::make_unique<GenericFactory<TestConverter>>(), "TestConverter");
  processor.Register(std::make_unique<GenericFactory<TestWriter>>(), "TestWriter");

  EXPECT_THROW(processor.Parse(xml_stream), std::runtime_error);
}

TEST(ColaParse, NoWriter) {
  auto xml_stream = std::stringstream(R"(<?xml version="1.0"?>
    <root>
        <generator name="TestGenerator"/>
    </root>
    )");

  MetaProcessor processor;
  processor.Register(std::make_unique<GenericFactory<TestGenerator>>(), "TestGenerator");

  EXPECT_THROW(processor.Parse(xml_stream), std::runtime_error);
}

TEST(ColaParse, MultipleGenerators) {
  auto xml_stream = std::stringstream(R"(<?xml version="1.0"?>
    <root>
        <generator name="TestGenerator"/>
        <generator name="TestGenerator"/>
        <writer name="TestWriter"/>
    </root>
    )");

  MetaProcessor processor;
  processor.Register(std::make_unique<GenericFactory<TestGenerator>>(), "TestGenerator");
  processor.Register(std::make_unique<GenericFactory<TestConverter>>(), "TestConverter");
  processor.Register(std::make_unique<GenericFactory<TestWriter>>(), "TestWriter");

  EXPECT_THROW(processor.Parse(xml_stream), std::runtime_error);
}

TEST(ColaParse, MultipleWriters) {
  auto xml_stream = std::stringstream(R"(<?xml version="1.0"?>
    <root>
        <generator name="TestGenerator"/>
        <writer name="TestWriter"/>
        <writer name="TestWriter"/>
    </root>
    )");

  MetaProcessor processor;
  processor.Register(std::make_unique<GenericFactory<TestGenerator>>(), "TestGenerator");
  processor.Register(std::make_unique<GenericFactory<TestConverter>>(), "TestConverter");
  processor.Register(std::make_unique<GenericFactory<TestWriter>>(), "TestWriter");

  EXPECT_THROW(processor.Parse(xml_stream), std::runtime_error);
}

TEST(ColaPipeline, RunPipeline) {
  auto ensemble = FilterEnsemble{
      .generator = std::make_unique<TestGenerator>(),
      .converters = MakeVector(std::unique_ptr<VConverter>(new TestConverter)),
      .writer = std::make_unique<TestWriter>(),
  };
  auto* generator_ptr = dynamic_cast<TestGenerator*>(ensemble.generator.get());
  auto* converter_ptr = dynamic_cast<TestConverter*>(ensemble.converters[0].get());
  auto* writer_ptr = dynamic_cast<TestWriter*>(ensemble.writer.get());

  ColaRunManager manager(std::move(ensemble));
  manager.Run(3);

  EXPECT_EQ(generator_ptr->call_counter, 3);
  EXPECT_EQ(converter_ptr->call_counter, 3);
  EXPECT_EQ(writer_ptr->call_counter, 3);
}

TEST(ColaTest, MultipleConverters) {
  auto ensemble = FilterEnsemble{
      .generator = std::make_unique<TestGenerator>(),
      .converters =
          MakeVector(std::unique_ptr<VConverter>(new TestConverter), std::unique_ptr<VConverter>(new TestConverter)),
      .writer = std::make_unique<TestWriter>(),
  };
  auto* generator_ptr = dynamic_cast<TestGenerator*>(ensemble.generator.get());
  auto* converter_ptr1 = dynamic_cast<TestConverter*>(ensemble.converters[0].get());
  auto* converter_ptr2 = dynamic_cast<TestConverter*>(ensemble.converters[1].get());
  auto* writer_ptr = dynamic_cast<TestWriter*>(ensemble.writer.get());

  ColaRunManager manager(std::move(ensemble));
  manager.Run(3);

  EXPECT_EQ(generator_ptr->call_counter, 3);
  EXPECT_EQ(converter_ptr1->call_counter, 3);
  EXPECT_EQ(converter_ptr2->call_counter, 3);
  EXPECT_EQ(writer_ptr->call_counter, 3);
}

TEST(ColaTest, StageHandling) {
  auto ensemble = FilterEnsemble{
      .generator = std::make_unique<TestGenerator>(),
      .converters = MakeVector(std::unique_ptr<VConverter>(new TestConverter)),
      .writer = std::make_unique<TestWriter>(),
  };
  auto* generator_ptr = dynamic_cast<TestGenerator*>(ensemble.generator.get());
  auto* converter_ptr = dynamic_cast<TestConverter*>(ensemble.converters[0].get());
  auto* writer_ptr = dynamic_cast<TestWriter*>(ensemble.writer.get());

  ColaRunManager manager(std::move(ensemble));
  manager.Run(1);

  EXPECT_EQ(generator_ptr->call_counter, 1);
  EXPECT_EQ(converter_ptr->call_counter, 1);
  EXPECT_EQ(writer_ptr->call_counter, 1);

  ASSERT_GE(writer_ptr->recorded_events.size(), 1);
  ASSERT_EQ(writer_ptr->recorded_events[0]->particles.size(), 1);
  EXPECT_EQ(writer_ptr->recorded_events[0]->particles[0].pdg_code, cola::AZToPdg({1, 0}));
}

TEST(ColaTest, ParseAndRun) {
  auto xml_stream = std::stringstream(R"(<?xml version="1.0"?>
    <root>
        <generator name="TestGenerator"/>
        <converter name="TestConverter"/>
        <writer name="TestWriter"/>
    </root>
    )");

  MetaProcessor processor;
  processor.Register(std::make_unique<GenericFactory<TestGenerator>>(), "TestGenerator");
  processor.Register(std::make_unique<GenericFactory<TestConverter>>(), "TestConverter");
  processor.Register(std::make_unique<GenericFactory<TestWriter>>(), "TestWriter");

  auto ensemble = processor.Parse(xml_stream);

  ASSERT_EQ(ensemble.converters.size(), 1);

  auto* generator_ptr = dynamic_cast<TestGenerator*>(ensemble.generator.get());
  auto* converter_ptr = dynamic_cast<TestConverter*>(ensemble.converters[0].get());
  auto* writer_ptr = dynamic_cast<TestWriter*>(ensemble.writer.get());

  ColaRunManager manager(std::move(ensemble));
  manager.Run(1);

  EXPECT_EQ(generator_ptr->call_counter, 1);
  EXPECT_EQ(converter_ptr->call_counter, 1);
  EXPECT_EQ(writer_ptr->call_counter, 1);

  ASSERT_GE(writer_ptr->recorded_events.size(), 1);
  ASSERT_EQ(writer_ptr->recorded_events[0]->particles.size(), 1);
  EXPECT_EQ(writer_ptr->recorded_events[0]->particles[0].pdg_code, cola::AZToPdg({1, 0}));
}
