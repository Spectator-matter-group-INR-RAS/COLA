//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_VGENERATOR_HH
#define GENERATORFRAMEWORK_VGENERATOR_HH

#include "EventData.hh"

namespace cola {

    class VGenerator {
    public:
        virtual ~VGenerator() = 0;
        virtual cola::EventData genEvent() = 0;
    };

    class GeneratorImp final : public VGenerator {
    public:
        GeneratorImp() = default;
        ~GeneratorImp() = default;
        cola::EventData genEvent() final {
            return cola::EventData{{5}, {5}};
        }
    };

    inline VGenerator::~VGenerator() = default;

} //cola


#endif //GENERATORFRAMEWORK_VGENERATOR_HH
