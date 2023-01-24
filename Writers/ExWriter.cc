//
// Created by apbus_amp_k on 16.01.23.
//

#include "ExWriter.hh"
#include <iostream>

namespace cola {

    void ExWriter::operator()(EventData data) {
        for(auto i : data.particles)
            std::cout << i.pdgCode << ' ';
        std::cout << std::endl;
    }

} // cola