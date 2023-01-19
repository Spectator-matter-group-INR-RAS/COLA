//
// Created by apbus_amp_k on 16.01.23.
//

#include "ExGenerator.hh"

namespace cola {

    EventData ExGenerator::operator()() {
        EventData data;
        unsigned short code = pdg_list(rng);
        data.iniState = {code,
                         code,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0,
                         0};
        data.particles.emplace_back(Particle{-5, -5, 0, 0, 0, 10000, code, ParticleClass::spectatorA});
        data.particles.emplace_back(Particle{-5, -5, 0, 0, 0, -10000, code, ParticleClass::spectatorB});
        return data;
    }

} // cola