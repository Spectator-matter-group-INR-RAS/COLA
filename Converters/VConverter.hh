#ifndef GENERATORFRAMEWORK_VCONVERTER_HH
#define GENERATORFRAMEWORK_VCONVERTER_HH

#include "EventData.hh"
#include "VFilter.hh"

/*
 * This is a converter interface. It is inherited by all filters that are in the middle of MC simulation.
 * Converters are intended to transform data from previous steps using map and reduce methods.
 */

namespace cola {

    class VConverter : public VFilter{
    public:
        ~VConverter() override = 0;

        virtual EventData operator()(EventData data) = 0;
    };

    inline VConverter::~VConverter() = default;

} //cola

#endif //GENERATORFRAMEWORK_VCONVERTER_HH
