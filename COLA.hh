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

#ifndef COLA_COLA_HH
#define COLA_COLA_HH

#include "EventData.hh"

#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tinyxml2 {
  class XMLDocument;
}  // namespace tinyxml2

namespace cola {
  /** \defgroup Interface Pure abstract classes used for dependency injection.
   * @{
   */

  /** A common abstract parent class representing any model in the pipeline.
   *  A single model in the COLA-driven pipeline is named a Filter.
   */
  class VFilter {
   public:
    VFilter() = default;
    VFilter(const VFilter&) = delete;
    VFilter(VFilter&&) = delete;
    VFilter& operator=(const VFilter&) = delete;
    VFilter& operator=(VFilter&&) = delete;
    virtual ~VFilter() = 0;
  };

  inline VFilter::~VFilter() = default;

  /** Generator abstract class.
   *  This is a generator interface. Generators are the first step of the MC simulation: they take data from existing
   *  files or encapsulate nucleus-nucleus collision generators.
   */
  class VGenerator : public VFilter {
   public:
    VGenerator() = default;
    VGenerator(const VGenerator&) = delete;
    VGenerator(VGenerator&&) = delete;
    VGenerator& operator=(const VGenerator&) = delete;
    VGenerator& operator=(VGenerator&&) = delete;
    ~VGenerator() override = 0;

    /** A method to request one event from the generator.
     *  Users are supposed to override this method to provide a single event from the generator model.
     *  @return A pointer to the EventData of the produced event.
     */
    virtual std::unique_ptr<EventData> operator()() = 0;
  };

  inline VGenerator::~VGenerator() = default;

  /** Converter abstract class.
   *  This is a converter interface. It is inherited by all filters that are in the middle of MC simulation.
   *  Converters are intended to transform data from previous steps.
   */
  class VConverter : public VFilter {
   public:
    VConverter() = default;
    VConverter(const VConverter&) = delete;
    VConverter(VConverter&&) = delete;
    VConverter& operator=(const VConverter&) = delete;
    VConverter& operator=(VConverter&&) = delete;
    ~VConverter() override = 0;

    /** A method to process one event by the converter model.
     *  Users are supposed to override this method to process a single event by the converter model.
     *  @param data A pointer to the EventData to be processed.
     *  @return A pointer to the EventData of the processed event.
     */
    virtual std::unique_ptr<EventData> operator()(std::unique_ptr<EventData>&& data) = 0;
  };

  inline VConverter::~VConverter() = default;

  /** Writer abstract class.
   *  This is a writer interface. Writers are what the name suggests - they implement writing results into different
   *  data formats. Note, that it isn't necessary to *only* write events to memory with this class. If you need to
   *  process data in a way that the EventData is incompatible with the output (for example, you want to model a
   *  detector response to the generated event) you can encapsulate these calculations in a writer class.
   */
  class VWriter : public VFilter {
   public:
    VWriter() = default;
    VWriter(const VWriter&) = delete;
    VWriter(VWriter&&) = delete;
    VWriter& operator=(const VWriter&) = delete;
    VWriter& operator=(VWriter&&) = delete;
    ~VWriter() override = 0;

    /** A method to save one event by the writer.
     *  Users are supposed to override this method to save a single event. Note, that the implementation is left
     *  to the user: you can save events to buffer with this method and write them to the filesystem in batches if
     *  you please so.
     *  @param data A pointer to the EventData to be saved.
     */
    virtual void operator()(std::unique_ptr<EventData>&& data) = 0;
  };

  inline VWriter::~VWriter() = default;

  /** An enum for marking Filter types.
   */
  enum class FilterType : char { kGenerator, kConverter, kWriter };

  /** Factory abstract class.
   * This is a factory interface. It generates a Filter with its VFactory::create method. DI in COLA works via using
   * the factory classes, which are registered in a MetaProcessor instance.
   */
  class VFactory {
   public:
    VFactory() = default;
    VFactory(const VFactory&) = delete;
    VFactory(VFactory&&) = delete;
    VFactory& operator=(const VFactory&) = delete;
    VFactory& operator=(VFactory&&) = delete;
    virtual ~VFactory() = default;

    /** The method for constructing filters.
     *  This method is intended to be called in MetaProcessor with key-value pairs obtained from a XML-file.
     *  @param metaData A dictionary with key-value pairs needed for configuring a model.
     *  @return A configured class that is a VFilter child.
     */
    virtual std::unique_ptr<VFilter> Create(const std::unordered_map<std::string, std::string>& meta_data) = 0;

    virtual FilterType GetFilterType() const = 0;

    virtual const std::string& GetFilterName() const = 0;
  };

  using FactoryMap = std::unordered_map<std::string, std::unique_ptr<VFactory>>;

  class VGeneratorFactory : public VFactory {
   public:
    FilterType GetFilterType() const final { return FilterType::kGenerator; }
  };

  class VConverterFactory : public VFactory {
   public:
    FilterType GetFilterType() const final { return FilterType::kConverter; }
  };

  class VWriterFactory : public VFactory {
   public:
    FilterType GetFilterType() const final { return FilterType::kWriter; }
  };

  template <typename Filter>
  class GenericFactory : public VFactory {
   private:
    template <typename, typename = void>
    struct HasName : std::false_type {};

    template <typename T>
    struct HasName<T, std::void_t<decltype(T::kName)>> : std::true_type {};

    template <typename T>
    static constexpr bool kHasName = HasName<T>::value;

   public:
    GenericFactory() {
      static_assert(std::is_base_of_v<VFilter, Filter>, "Filter template must inherit cola::VFilter");
      static_assert(kHasName<Filter>, "Filter class must have kName class variable");
      static_assert(std::is_same_v<std::remove_reference_t<std::remove_cv_t<decltype(Filter::kName)>>, std::string>,
                    "kName variable must be a string");
    }

    std::unique_ptr<VFilter> Create(const std::unordered_map<std::string, std::string>& meta_data) override {
      if constexpr (std::is_constructible_v<Filter, decltype(meta_data)>) {
        return std::make_unique<Filter>(meta_data);
      } else {
        return std::make_unique<Filter>();
      }
    }

    const std::string& GetFilterName() const final { return Filter::kName; }

    FilterType GetFilterType() const final {
      if constexpr (std::is_base_of_v<VGenerator, Filter>) {
        return FilterType::kGenerator;
      } else if constexpr (std::is_base_of_v<VConverter, Filter>) {
        return FilterType::kConverter;
      } else if constexpr (std::is_base_of_v<VWriter, Filter>) {
        return FilterType::kWriter;
      } else {
        static_assert(true, "unhandled 'FilterType' value");
      }
    }
  };

  std::unique_ptr<EventData> operator|(const std::unique_ptr<VGenerator>& /*generator*/,
                                       const std::unique_ptr<VConverter>& /*converter*/);
  std::unique_ptr<EventData> operator|(std::unique_ptr<EventData>&& /*data*/,
                                       const std::unique_ptr<VConverter>& /*converter*/);
  void operator|(std::unique_ptr<EventData>&& /*data*/, const std::unique_ptr<VWriter>& /*writer*/);

  /** @}
   *  \defgroup Metadata Classes for constructing and running a model.
   *  @{
   */

  /** A structure representing the model pipeline.
   */
  struct FilterEnsemble {
    std::unique_ptr<VGenerator> generator;               /**< Event generator. */
    std::vector<std::unique_ptr<VConverter>> converters; /**< Vector of converters, applied step-by-step. */
    std::unique_ptr<VWriter> writer;                     /**< Writer to save the results. */
  };

  /** A class for processing meta information.
   *  This class stores data about all available Filters and corresponding factory classes and uses it to create the
   *  pipeline using its MetaProcessor:parse method to read all needed information from an XML-file.
   */
  class MetaProcessor {
   public:
    /** Default constructor.
     */
    MetaProcessor() = default;

    /** Constructor with immediate factories registration.
     * Note that unique pointers in the dictionary are invalidated.
     * @param filterMap A dictionary with all relevant information. Note that unique pointers in the dictionary are
     * invalidated.
     */
    explicit MetaProcessor(FactoryMap&& filter_map);

    ~MetaProcessor() = default;

    /** A method for registering new Filters.
     * @param factory A rvalue-reference to the Filter factory pointer
     * @param name The name of the Filter.
     */
    void Register(std::unique_ptr<VFactory>&& factory, const std::optional<std::string>& name = std::nullopt);

    /** A method to parse a XML-file to set up a configured FilterEnsemble.
     *  This method opens an XML-file @param file_name to get the information to set up the model.
     *  Inside the root element should be one <generator> element followed by any number of <converter> elements
     *  (none is possible) and, finally, a <writer> element. Each element must have "name" attribute followed by
     *  any number of additional attributes. These attributes are then passed to the corresponding factory's
     * VFactory::create method as a dictionary with keys being attribute names and values - attribute values. This
     * method throws an error if a relevant factory isn't found or XML-file is malformed.
     *  @param file_name Name with the configuration XML-file.
     *  @return A configured FilterEnsemble.
     */
    FilterEnsemble Parse(const std::string& file_name) const;

    /** A method to parse a XML-file to set up a configured FilterEnsemble.
     *  @param contents configuration XML-file contents
     *  @return A configured FilterEnsemble.
     */
    FilterEnsemble Parse(std::istream& contents) const;

   private:
    FactoryMap generator_map_;
    FactoryMap converter_map_;
    FactoryMap writer_map_;

    FilterEnsemble BuildFilterEnsemble(const tinyxml2::XMLDocument& document) const;

    void RegisterGenerator(std::unique_ptr<VFactory>&& factory, const std::string& name) {
      generator_map_.emplace(name, std::move(factory));
    }
    void RegisterConverter(std::unique_ptr<VFactory>&& factory, const std::string& name) {
      converter_map_.emplace(name, std::move(factory));
    }
    void RegisterWriter(std::unique_ptr<VFactory>&& factory, const std::string& name) {
      writer_map_.emplace(name, std::move(factory));
    }
  };

  /** Manager class.
   * Currently more of a boilerplate, but potentially useful to incorporate parallel computing.
   */
  class ColaRunManager {
   public:
    /** A constructor that moves the configured FilterEnsemble into the manager.
     * @param ensemble Configured model.
     */
    explicit ColaRunManager(FilterEnsemble&& ensemble) : filter_ensamble_(std::move(ensemble)) {}

    ~ColaRunManager() = default;

    /** A method to run the resulting model @param n times.
     * @param n Number of runs.
     */
    void Run(int n = 1) const;

   private:
    FilterEnsemble filter_ensamble_;
  };

  class VModule {
   public:
    VModule() = default;

    FactoryMap GetModuleFilters(const std::optional<std::string>& prefix = std::nullopt) const;

    virtual ~VModule() = default;

   private:
    virtual FactoryMap DoGetModuleFilters() const = 0;
  };

  template <typename... FilterTypes>
  class GenericModule : public VModule {
   private:
    template <typename HeadType, typename... Types>
    static void AddFilter(FactoryMap& factories) {
      static_assert(std::is_base_of_v<VFactory, HeadType>, "all types must be Factories");

      {
        auto factory = std::make_unique<HeadType>();
        factories[factory->GetFilterName()] = std::move(factory);
      }

      if constexpr (sizeof...(Types) > 0) {
        AddFilter<Types...>(factories);
      }
    }

    FactoryMap DoGetModuleFilters() const override {
      FactoryMap factories;
      AddFilter<FilterTypes...>(factories);
      return factories;
    }
  };

  std::unique_ptr<cola::VModule> LoadModule(const std::string& module_name,
                                            const std::optional<std::string>& lib_directory = std::nullopt);
}  // namespace cola

extern "C" {
/** Loads COLA module in runtime, shouldn't be used directly
 * @return A cola::VModule class wrapping specific COLA module
 */
cola::VModule* LoadCOLAModule();
}

#endif  // COLA_COLA_HH
