//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_VCONVERTER_HH
#define GENERATORFRAMEWORK_VCONVERTER_HH
#include "../Management/EventData.hh"
#include "VFilter.hh"

namespace cola {

    class VConverter : VFilter{
    public:
        virtual cola::EventData operator()(cola::EventData data) = 0;
        virtual ~VConverter() = 0;
    };

    inline VConverter::~VConverter() = default;
}
#endif //GENERATORFRAMEWORK_VCONVERTER_HH
