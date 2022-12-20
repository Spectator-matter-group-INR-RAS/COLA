#ifndef GENERATORFRAMEWORK_EVENTDATA_HH
#define GENERATORFRAMEWORK_EVENTDATA_HH

#include <vector>

/*
 * A collection of DTOs and utility functions used in the pipeline
 */

namespace cola {

    //what happened to particle after generation
    enum class ParticleClass: char {
        produced,
        elasticA,
        elasticB,
        nonelasticA,
        nonelasticB,
        spectatorA,
        spectatorB
    };

    //initial nuclei nad kinematics, diagnostical information
    struct EventIniState{
        unsigned short pdgCodeA;
        unsigned short pdgCodeB;

        double pZA;
        double pZB;
        double energy;      //per nucleon or sqrtSnn, depends on pZB being zero

        float sectNN;
        float b;

        int nColl;
        int nCollPP;
        int nCollPN;
        int nCollNN;
        int nPart;
        int nPartA;
        int nPartB;

        float phiRotA;
        float thetaRotA;
        float phiRotB;
        float thetaRotB;
    };

    //any particle
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

    //A and Z of an ion
    typedef std::pair<unsigned short, unsigned short> AZ;
} //cola

#endif //GENERATORFRAMEWORK_EVENTDATA_HH
