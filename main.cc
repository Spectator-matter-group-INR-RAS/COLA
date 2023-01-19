#include <iostream>
#include <memory>

#include "ColaRunManager.hh"

#include "ExConvFac.hh"
#include "ExGenFac.hh"
#include "ExWriterFac.hh"

int main() {
    //create a metaprocessor
    cola::MetaProcessor metaProcessor;

    //register all needed factories
    metaProcessor.reg(std::make_shared<cola::ExGenFac>(), "example", "generator");
    metaProcessor.reg(std::make_shared<cola::ExConvFac>(), "example", "converter");
    metaProcessor.reg(std::make_shared<cola::ExWriterFac>(), "example", "writer");

    //get metadata (input)
    cola::MetaData exData = {"example", "example", "example"};

    //create a manager, using metaprocessor and metadata TODO: make some universal parser from an input string
    cola::ColaRunManager manager(metaProcessor.parse(exData));

    //use ColaRunManager::run() to run a single experiment
    std::cout<<"Test run: "<<std::endl;
    for (int i = 0; i < 10; i++)
        manager.run();
    std::cout << "End test" << std::endl;

    return 0;
}
