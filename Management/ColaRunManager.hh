//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_COLARUNMANAGER_HH
#define GENERATORFRAMEWORK_COLARUNMANAGER_HH
#include <iostream>
#include <map>
#include <memory>
#include <functional>
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
        std::shared_ptr<std::function<EventData()>> generator;
        std::shared_ptr<std::function<EventData(EventData)>> converter;
        std::shared_ptr<std::function<void(EventData)>> writer;
    };

    class MetaProcessor {
    public:
        MetaProcessor() = default;
        ~MetaProcessor();
        void reg(VFactory* factory, std::string name, std::string type);
        FilterAnsamble parse(MetaData data);

    private:
        VGenerator* gen;
        VConverter* conv;
        VWriter* write;

        std::map<std::string, VFactory*> generatorMap;
        std::map<std::string, VFactory*> converterMap;
        std::map<std::string, VFactory*> writerMap;

        void regGen(VFactory* factory, std::string name){generatorMap.emplace(name, factory);}
        void regConv(VFactory* factory, std::string name){converterMap.emplace(name, factory);}
        void regWrite(VFactory* factory, std::string name){writerMap.emplace(name, factory);}
    };

    class ColaRunManager {
    public:
        ColaRunManager(FilterAnsamble ansamble){filterAnsamble = ansamble;};
        ~ColaRunManager();
        void run();
    private:
        FilterAnsamble filterAnsamble;
    };

} //cola

#endif //GENERATORFRAMEWORK_COLARUNMANAGER_HH
