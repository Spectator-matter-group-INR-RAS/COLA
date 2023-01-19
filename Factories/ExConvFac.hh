//
// Created by apbus_amp_k on 16.01.23.
//

#ifndef GENERATORFRAMEWORK_EXCONVFAC_HH
#define GENERATORFRAMEWORK_EXCONVFAC_HH

#include "VFactory.hh"
#include "ExConverter.hh"

namespace cola {

    class ExConvFac final : public VFactory{
    public:
        ~ExConvFac() final = default;

        VFilter* create() final { return new ExConverter; }
    };

} // cola

#endif //GENERATORFRAMEWORK_EXCONVFAC_HH
