#ifndef GENERATORFRAMEWORK_VGENERATOR_HH
#define GENERATORFRAMEWORK_VGENERATOR_HH

#include "EventData.hh"
#include "VFilter.hh"

/*
 * This is a generator interface. Generators are the first step of the MC simulation: they take data from existing
 * files or encapsulate nucleus-nucleus collision generators.
 */

namespace cola {

    class VGenerator : VFilter{
    public:
        ~VGenerator() override = 0;
        virtual EventData operator()() = 0;
    };

    inline VGenerator::~VGenerator() = default;

} //cola

#endif //GENERATORFRAMEWORK_VGENERATOR_HH
