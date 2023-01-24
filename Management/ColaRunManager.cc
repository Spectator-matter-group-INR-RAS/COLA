//
// Created by alexsvetlichnyy on 20.12.22.
//

#include "ColaRunManager.hh"

cola::FilterAnsamble cola::MetaProcessor::parse(const cola::MetaData& data) {
    FilterAnsamble ansamble;
    ansamble.generator = std::unique_ptr<VGenerator>(dynamic_cast<VGenerator*>(generatorMap.at(data.generatorName)->create()));
    ansamble.converter = std::unique_ptr<VConverter>(dynamic_cast<VConverter*>(converterMap.at(data.converterName)->create()));
    ansamble.writer = std::unique_ptr<VWriter>(dynamic_cast<VWriter*>(writerMap.at(data.writerName)->create()));
    return ansamble;
}

void cola::MetaProcessor::reg(std::shared_ptr<VFactory> factory, const std::string& name, const std::string& type) {
    if(type == "generator"){ regGen(std::move(factory), name);}
    else if(type == "converter"){ regConv(std::move(factory), name);}
    else if(type == "writer"){ regWrite(std::move(factory), name);}
    else{std::cerr<<"ERROR in MetaProcessor: No such type of filter";}
}

void cola::ColaRunManager::run() {
    (*(filterAnsamble.writer))((*(filterAnsamble.converter))((*(filterAnsamble.generator))()));
}
