//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_VCONVERTER_HH
#define GENERATORFRAMEWORK_VCONVERTER_HH
#include "../Management/EventData.hh"

namespace cola {

class VConverter {
public:
    virtual cola::EventData operator()(cola::EventData data) = 0;
    virtual ~VConverter() = 0;
};

<<<<<<< HEAD
} //cola
=======
inline VConverter::~VConverter()= default;

>>>>>>> 84f3f67 (smth)

#endif //GENERATORFRAMEWORK_VCONVERTER_HH
