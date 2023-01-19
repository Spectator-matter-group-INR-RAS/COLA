//
// Created by apbus_amp_k on 16.01.23.
//

#ifndef GENERATORFRAMEWORK_EXGENFAC_HH
#define GENERATORFRAMEWORK_EXGENFAC_HH

#include "VFactory.hh"
#include "ExGenerator.hh"

namespace cola {

    class ExGenFac final : public VFactory {
    public:
        ~ExGenFac() final = default;

        VFilter* create() final { return new ExGenerator; }
    };

} // cola

#endif //GENERATORFRAMEWORK_EXGENFAC_HH
