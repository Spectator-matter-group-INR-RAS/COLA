//
// Created by alexsvetlichnyy on 20.12.22.
//

#include "ColaRunManager.hh"

cola::MetaProcessor::~MetaProcessor() {
delete write; delete conv; delete gen;
}

cola::FilterAnsamble cola::MetaProcessor::parse(cola::MetaData data) {
    FilterAnsamble ansamble;
    gen = dynamic_cast<VGenerator*>(generatorMap.at(data.generatorName)->create().get());
    ansamble.generator = std::make_shared<std::function<EventData()>>([&](){return (*gen)();});
    conv = dynamic_cast<VConverter*>(converterMap.at(data.converterName)->create().get());
    ansamble.converter = std::make_shared<std::function<EventData(EventData)>>([&](EventData  eventData){return (*conv)(eventData);});
    write = dynamic_cast<VWriter*>(writerMap.at(data.writerName)->create().get());
    ansamble.writer = std::make_shared<std::function<void(EventData)>>([&](EventData eventData){return (*write)(eventData);});
    return ansamble;
}

void cola::MetaProcessor::reg(cola::VFactory *factory, std::string name, std::string type) {
    if(type == "generator"){ regGen(factory, name);}
    else if(type == "converter"){ regConv(factory, name);}
    else if(type == "writer"){ regWrite(factory, name);}
    else{std::cerr<<"ERROR in MetaProcessor: No such type of filter";}
}

void cola::ColaRunManager::run() {
    (*filterAnsamble.writer)((*filterAnsamble.converter)((*filterAnsamble.generator)()));
}
