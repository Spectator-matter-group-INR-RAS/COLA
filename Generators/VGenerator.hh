//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_VGENERATOR_HH
#define GENERATORFRAMEWORK_VGENERATOR_HH

#include "EventData.hh"

class VGenerator {
public:
    virtual ~VGenerator() = 0;
    virtual EventData genEvent() = 0;
};

class GeneratorImp final : public VGenerator {
public:
    GeneratorImp() = default;
    ~GeneratorImp() = default;
    EventData genEvent() final {
        return EventData{{5}, {5}};
    }
};

inline VGenerator::~VGenerator() = default;


#endif //GENERATORFRAMEWORK_VGENERATOR_HH
