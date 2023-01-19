//
// Created by apbus_amp_k on 16.01.23.
//

#ifndef GENERATORFRAMEWORK_EXWRITERFAC_HH
#define GENERATORFRAMEWORK_EXWRITERFAC_HH

#include "VFactory.hh"
#include "ExWriter.hh"

namespace cola {

    class ExWriterFac final : public VFactory {
    public:
        ~ExWriterFac() final = default;

        VFilter* create() final { return new ExWriter; }
    };

} // cola

#endif //GENERATORFRAMEWORK_EXWRITERFAC_HH
