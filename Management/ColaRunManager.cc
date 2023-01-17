//
// Created by alexsvetlichnyy on 20.12.22.
//

#include "ColaRunManager.hh"

cola::MetaProcessor::~MetaProcessor() {
delete write; delete conv; delete gen;
}

cola::FilterAnsamble cola::MetaProcessor::parse(const cola::MetaData& data) {
    FilterAnsamble ansamble;
    ansamble.generator = std::unique_ptr<VGenerator>(dynamic_cast<VGenerator*>(generatorMap.at(data.generatorName)->create().get()));
    ansamble.converter = std::unique_ptr<VConverter>(dynamic_cast<VConverter*>(converterMap.at(data.converterName)->create().get()));
    ansamble.writer = std::unique_ptr<VWriter>(dynamic_cast<VWriter*>(writerMap.at(data.writerName)->create().get()));
    return ansamble;
}

void cola::MetaProcessor::reg(cola::VFactory *factory, const std::string& name, const std::string& type) {
    if(type == "generator"){ regGen(factory, name);}
    else if(type == "converter"){ regConv(factory, name);}
    else if(type == "writer"){ regWrite(factory, name);}
    else{std::cerr<<"ERROR in MetaProcessor: No such type of filter";}
}

void cola::ColaRunManager::run() {
    (*filterAnsamble.writer)((*filterAnsamble.converter)((*filterAnsamble.generator)()));
}
