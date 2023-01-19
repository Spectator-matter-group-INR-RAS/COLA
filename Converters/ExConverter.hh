#ifndef GENERATORFRAMEWORK_EXCONVERTER_HH
#define GENERATORFRAMEWORK_EXCONVERTER_HH

#include "VConverter.hh"

// an example Converter class

namespace cola {

    class ExConverter final : public VConverter {
    public:
        ExConverter() = default;
        ~ExConverter() final = default;

        EventData operator()(EventData data) final;
    };

} // cola

#endif //GENERATORFRAMEWORK_EXCONVERTER_HH
