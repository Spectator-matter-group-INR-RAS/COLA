//
// Created by apbus_amp_k on 16.01.23.
//

#ifndef GENERATORFRAMEWORK_EXWRITER_HH
#define GENERATORFRAMEWORK_EXWRITER_HH

#include "VWriter.hh"

namespace cola {

    class ExWriter final : public VWriter{
    public:
        ExWriter() = default;
        ~ExWriter() final = default;

        void operator()(EventData) final;
    };

} // cola

#endif //GENERATORFRAMEWORK_EXWRITER_HH
