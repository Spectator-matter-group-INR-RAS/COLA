//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_VFACTORY_HH
#define GENERATORFRAMEWORK_VFACTORY_HH

#include "VFilter.hh"
#include <memory>

namespace cola {
    class VFactory{
        virtual VFilter create() = 0;
        virtual ~VFactory();
    };
    inline VFactory::~VFactory() = default;
}
#endif //GENERATORFRAMEWORK_VFACTORY_HH
