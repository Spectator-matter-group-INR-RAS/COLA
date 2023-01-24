//
// Created by apbus_amp_k on 16.01.23.
//

#ifndef GENERATORFRAMEWORK_EXGENERATOR_HH
#define GENERATORFRAMEWORK_EXGENERATOR_HH

#include "VGenerator.hh"
#include <random>

namespace cola {

    class ExGenerator final : public VGenerator {
    public:
        ExGenerator() : rng(std::random_device()()),
        pdg_list(std::uniform_int_distribution<unsigned short>(1, 10000)) {};
        ~ExGenerator() final = default;

        EventData operator()() final;
    private:
        std::mt19937 rng;
        std::uniform_int_distribution<unsigned short> pdg_list;
    };

} // cola

#endif //GENERATORFRAMEWORK_EXGENERATOR_HH
