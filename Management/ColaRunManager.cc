//
// Created by alexsvetlichnyy on 20.12.22.
//

#include "ColaRunManager.hh"

cola::FilterAnsamble cola::MetaProcessor::parse(cola::MetaData data) {
    return cola::FilterAnsamble();
}

void cola::MetaProcessor::reg(cola::VFactory *factory, std::string name, std::string type) {
    if(type == "generator"){ regGen(factory, name);}
    else if(type == "converter"){ regConv(factory, name);}
    else if(type == "writer"){ regWrite(factory, name);}
    else{std::cerr<<"ERROR in MetaProcessor: No such type of filter";}
}

