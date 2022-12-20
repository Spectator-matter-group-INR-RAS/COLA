//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_RUNMANAGER_HH
#define GENERATORFRAMEWORK_RUNMANAGER_HH
#include <iostream>
#include "../Generators/VGenerator.hh"
#include "../Writers/VWriter.hh"
#include "../Converters/VConverter.hh"
class RunManager {
public:
    RunManager();
    ~RunManager();
    /*
    void add(VGenerator generator,std::string fTypeMeta);
    void add(VWriter writer, std::string fTypeMeta);
    void add(VConverter converter, std::string fTypeMeta);*/

private:


};


#endif //GENERATORFRAMEWORK_RUNMANAGER_HH
