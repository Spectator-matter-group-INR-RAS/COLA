//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_EVENTDATA_HH
#define GENERATORFRAMEWORK_EVENTDATA_HH

#include <vector>

namespace cola {

    enum class ParticleClass: char {
        produced,
        elasticA,
        elasticB,
        nonelasticA,
        nonelasticB,
        spectatorA,
        spectatorB
    };

    struct EventIniState{
        unsigned short pdgCodeA;
        unsigned short pdgCodeB;

        double pZA;
        double pZB;
        double sqrtSNN;

        float sectNN;
    };

    struct Particle {
        double x;
        double y;
        double z;

        double pX;
        double pY;
        double pZ;

        unsigned short pdgCode;
        ParticleClass pClass;
    };

    typedef std::vector<Particle> EventParticles;

    class EventData{
    public:
        EventIniState iniState;
        EventParticles particles;
    };

} //cola

#endif //GENERATORFRAMEWORK_EVENTDATA_HH
