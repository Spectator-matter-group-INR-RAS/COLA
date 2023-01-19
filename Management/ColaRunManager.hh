//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_COLARUNMANAGER_HH
#define GENERATORFRAMEWORK_COLARUNMANAGER_HH
#include <iostream>
#include <map>
#include <memory>
#include <functional>
#include <utility>
#include "VGenerator.hh"
#include "VWriter.hh"
#include "VConverter.hh"
#include "VFactory.hh"

namespace cola {

    struct MetaData {
        std::string generatorName;
        std::string converterName;
        std::string writerName;
    };

    struct FilterAnsamble{
        std::unique_ptr<VGenerator> generator;
        std::unique_ptr<VConverter> converter;
        std::unique_ptr<VWriter> writer;
    };

    class MetaProcessor { //TODO: make everything initialise in constructor
    public:
        MetaProcessor() = default;
        ~MetaProcessor() = default;

        void reg(std::shared_ptr<VFactory> factory, const std::string& name, const std::string& type);
        FilterAnsamble parse(const MetaData& data);

    private:
        std::map<std::string, std::shared_ptr<VFactory>> generatorMap;
        std::map<std::string, std::shared_ptr<VFactory>> converterMap;
        std::map<std::string, std::shared_ptr<VFactory>> writerMap;

        void regGen(std::shared_ptr<VFactory>&& factory, const std::string& name){generatorMap.emplace(name, std::move(factory));}
        void regConv(std::shared_ptr<VFactory>&& factory, const std::string& name){converterMap.emplace(name, std::move(factory));}
        void regWrite(std::shared_ptr<VFactory>&& factory, const std::string& name){writerMap.emplace(name, std::move(factory));}
    };

    class ColaRunManager {
    public:
        explicit ColaRunManager(FilterAnsamble&& ansamble) : filterAnsamble(std::move(ansamble)) {}
        ~ColaRunManager() = default;
        void run();
    private:
        FilterAnsamble filterAnsamble;
    };

} //cola

#endif //GENERATORFRAMEWORK_COLARUNMANAGER_HH
