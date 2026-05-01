/**
 * Copyright (c) 2024-2025 Alexandr Svetlichnyi, Savva Savenkov, Artemii Novikov
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

#include "COLA.hh"

#include <tinyxml2.h>

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
  std::map<std::string, std::string> GetNameAndParams(const tinyxml2::XMLElement* element, std::string& name) {
    const auto* current_attribute = element->FindAttribute("name");
    name = current_attribute->Value();
    std::cout << "filter name: " + name + "\nparams:\n";
    current_attribute = current_attribute->Next();
    std::map<std::string, std::string> params;
    while (current_attribute != nullptr) {
      params.emplace(current_attribute->Name(), current_attribute->Value());
      std::cout << current_attribute->Name() << ": " << current_attribute->Value() << '\n';
      current_attribute = current_attribute->Next();
    }
    return params;
  }
}  // namespace

namespace cola {

  // converters

  AZ PdgToAz(int pdgCode) {
    switch (pdgCode) {
      case 2112:
        return {1, 0};
      case 2212:
        return {1, 1};
      default: {
        AZ data = {0, 0};
        pdgCode /= 10;
        for (int i = 0; i < 3; i++) {
          data.first += pdgCode % 10 * static_cast<std::uint16_t>(pow(10, i));
          pdgCode /= 10;
        }
        for (int i = 0; i < 3; i++) {
          data.second += pdgCode % 10 * static_cast<std::uint16_t>(pow(10, i));
          pdgCode /= 10;
        }
        return data;
      }
    }
  }

  int AZToPdg(AZ data) {
    if (data.first == 1 && data.second == 0) {
      return 2112;
    }
    if (data.first == 1 && data.second == 1) {
      return 2212;
    }
    return 1000000000 + data.first * 10 + data.second * 10000;
  }

  AZ Particle::GetAz() const { return PdgToAz(pdgCode); }

  // operators

  std::unique_ptr<EventData> operator|(const std::unique_ptr<VGenerator>& generator,
                                       const std::unique_ptr<VConverter>& converter) {
    return (*converter)((*generator)());
  }
  std::unique_ptr<EventData> operator|(std::unique_ptr<EventData>&& data,
                                       const std::unique_ptr<VConverter>& converter) {
    return (*converter)(std::move(data));
  }
  void operator|(std::unique_ptr<EventData>&& data, const std::unique_ptr<VWriter>& writer) {
    (*writer)(std::move(data));
  }

  // MetaProcessor

  MetaProcessor::MetaProcessor(std::map<std::string, std::pair<std::unique_ptr<VFactory>, FilterType>>& filterMap) {
    for (auto& item : filterMap) {
      Reg(std::move(item.second.first), item.first, item.second.second);
    }
  }

  void MetaProcessor::Reg(std::unique_ptr<VFactory>&& factory, const std::string& name, const FilterType type) {
    switch (type) {
      case FilterType::kGenerator:
        RegGen(std::move(factory), name);
        break;
      case FilterType::kConverter:
        RegConv(std::move(factory), name);
        break;
      case FilterType::kWriter:
        RegWrite(std::move(factory), name);
        break;
      default:
        throw std::domain_error("ERROR in MetaProcessor: No such type of filter.");
    }
  }

  FilterEnsemble MetaProcessor::Parse(const std::string& fName) const {
    using namespace tinyxml2;
    std::cout << "Parsing XML file:" << '\n';
    XMLDocument file;
    auto code = file.LoadFile(fName.c_str());
    if (code != XML_SUCCESS) {
      throw std::runtime_error("ERROR in MetaProcessor: Couldn't open file `" + fName +
                               "`.\nError code (tinyxml2): " + std::to_string(code));
    }

    FilterEnsemble ensemble;

    auto* current_element = file.RootElement()->FirstChildElement("generator");
    std::string name;
    auto params = GetNameAndParams(current_element, name);
    ensemble.generator = std::unique_ptr<VGenerator>(dynamic_cast<VGenerator*>(generatorMap_.at(name)->Create(params)));
    params.clear();

    current_element = current_element->NextSiblingElement();
    while (current_element->Name() != std::string("writer")) {
      params = GetNameAndParams(current_element, name);
      ensemble.converters.push_back(
          std::unique_ptr<VConverter>(dynamic_cast<VConverter*>(converterMap_.at(name)->Create(params))));
      params.clear();
      current_element = current_element->NextSiblingElement();
    }

    params = GetNameAndParams(current_element, name);
    ensemble.writer = std::unique_ptr<VWriter>(dynamic_cast<VWriter*>(writerMap_.at(name)->Create(params)));
    return ensemble;
  }

  // Run manager

  void ColaRunManager::Run(int n) const {
    for (int k = 0; k < n; k++) {
      auto event = (*(filterEnsemble_.generator))();
      for (const auto& converter : filterEnsemble_.converters) {
        event = std::move(event) | converter;
      }
      std::move(event) | filterEnsemble_.writer;
    }
  }

}  // namespace cola
