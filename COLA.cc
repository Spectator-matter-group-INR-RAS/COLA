//
// Created by _amp_ on 7/11/24.
//

#include "COLA.hh"

#include <tinyxml2.h>

namespace cola {

    // converters

    AZ pdgToAZ(int pdgCode) {
        AZ data = {0, 0};
        pdgCode /= 10;
        for (int i = 0; i < 3; i++) {
            data.first += pdgCode % 10 * static_cast<unsigned short>(pow(10, i));
            pdgCode /= 10;
        }
        for (int i = 0; i < 3; i++) {
            data.second += pdgCode % 10 * static_cast<unsigned short>(pow(10, i));
            pdgCode /= 10;
        }
        return data;
    }

    int AZToPdg(AZ data) {
        return 1000000000 + data.first * 10 + data.second * 10000;
    }

    // operators

    std::unique_ptr<EventData> operator|(const std::unique_ptr<VGenerator>& generator, const std::unique_ptr<VConverter>& converter) {
        return (*converter)((*generator)());
    }
    std::unique_ptr<EventData> operator|(std::unique_ptr<EventData>&& data, const std::unique_ptr<VConverter>& converter) {
        return (*converter)(std::move(data));
    }
    void operator|(std::unique_ptr<EventData>&& data, const std::unique_ptr<VWriter>& writer) {
        (*writer)(std::move(data));
    }

    // Metaprocessor

    MetaProcessor::MetaProcessor(std::map<std::string, std::pair<std::unique_ptr<VFactory>, FilterType>>& filterMap) {
        for (auto& item: filterMap)
            reg(std::move(item.second.first), item.first, item.second.second);
    }

    void MetaProcessor::reg(std::unique_ptr<VFactory>&& factory, const std::string& name, const FilterType type) {
        switch (type) {
            case FilterType::generator:
                regGen(std::move(factory), name);
                break;
            case FilterType::converter:
                regConv(std::move(factory), name);
                break;
            case FilterType::writer:
                regWrite(std::move(factory), name);
                break;
            default:
                throw std::domain_error("ERROR in MetaProcessor: No such type of filter.");
        }
    }

    std::map<std::string, std::string> _get_name_and_params (const tinyxml2::XMLElement* element, std::string& name) {
        auto currentAttribute = element->FindAttribute("name");
        name = currentAttribute->Value();
        std::cout << "filter name: " + name +"\nparams:\n";
        currentAttribute = currentAttribute->Next();
        std::map<std::string, std::string> params;
        while (currentAttribute != nullptr){
            params.emplace(currentAttribute->Name(), currentAttribute->Value());
            std::cout << currentAttribute->Name() << ": " << currentAttribute->Value() << '\n';
            currentAttribute = currentAttribute->Next();
        }
        return params;
    }

    FilterEnsemble MetaProcessor::parse(const std::string &fname) const {
        using namespace tinyxml2;
        std::cout << "Parsing XML file:" << '\n';
        XMLDocument file;
        auto code = file.LoadFile(fname.c_str());
        if (code == XML_SUCCESS) {
            FilterEnsemble ensemble;

            auto currentElement = file.RootElement()->FirstChildElement("generator");
            std::string name;
            std::map<std::string, std::string> params = _get_name_and_params(currentElement, name);
            ensemble.generator = std::unique_ptr<VGenerator>(dynamic_cast<VGenerator*>(generatorMap.at(name)->create(params)));
            params.clear();

            currentElement = currentElement->NextSiblingElement();
            while (currentElement->Name() != std::string("writer")) {
                params = _get_name_and_params(currentElement, name);
                ensemble.converters.push_back(std::unique_ptr<VConverter>(dynamic_cast<VConverter*>(converterMap.at(name)->create(params))));
                params.clear();
                currentElement = currentElement->NextSiblingElement();
            }

            params = _get_name_and_params(currentElement, name);
            ensemble.writer = std::unique_ptr<VWriter>(dynamic_cast<VWriter*>(writerMap.at(name)->create(params)));
            return ensemble;
        } else {
            throw std::runtime_error("ERROR in MetaProcessor: Couldn't open XML file " + fname + "\nError code: " +
                                             std::to_string(code));
        }
    }

    // Run manager

    void ColaRunManager::run(int n) const {
        for(int k = 0; k < n; k++) {
            auto event = (*(filterEnsemble.generator))();
            for (const auto& converter : filterEnsemble.converters)
                event = std::move(event) | converter;
            std::move(event) | filterEnsemble.writer;
        }
    }

} //cola