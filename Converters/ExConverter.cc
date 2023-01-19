//
// Created by apbus_amp_k on 16.01.23.
//

#include "ExConverter.hh"

namespace cola {

    EventData ExConverter::operator()(EventData data) {
        data.iniState.pdgCodeA++;
        data.iniState.pdgCodeB--;

        bool p = true;
        for (auto& i : data.particles) {
            if (p) {
                i.pdgCode++;
                p = false;
            } else {
                i.pdgCode--;
                p = true;
            }
        }
        return data;
    }

} // cola