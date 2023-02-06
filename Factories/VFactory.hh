#ifndef GENERATORFRAMEWORK_VFACTORY_HH
#define GENERATORFRAMEWORK_VFACTORY_HH

#include "VFilter.hh"
#include <memory>

/*
 * This is a factory interface. Factories are classes used to create desired instances of filters. This abstraction
 * is required to implement dependency injection. Factories are registered in the metaprocessor class
 */

namespace cola {

    class VFactory{
    public:
        virtual ~VFactory();
        virtual VFilter* create() = 0;      //the pointer is passed to uniqEue_ptr constructor, therefore no leaks are possible
    };

    inline VFactory::~VFactory() = default;

} //cola

#endif //GENERATORFRAMEWORK_VFACTORY_HH
