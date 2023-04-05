#ifndef COLA_COLA_HH
#define COLA_COLA_HH

#include <map>
#include <memory>
#include <vector>
#include <cmath>

namespace cola {

    /* ----------------------- DATA ----------------------- */

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

        int pdgCode;
        ParticleClass pClass;
    };

    typedef std::vector<Particle> EventParticles;

    struct EventData{
        EventIniState iniState;
        EventParticles particles;
    };

    //A and Z of an ion
    typedef std::pair<unsigned short, unsigned short> AZ;

    //converters
    inline AZ pdgToAZ (int pdgCode) {
        AZ data = {0, 0};
        pdgCode /=10;
        for (int i = 0; i < 3; i++) {
            data.first += pdgCode % 10 * static_cast<unsigned short>(pow(10, i));
            pdgCode /= 10;
        }
        for (int i = 0; i < 3; i++) {
            data.second += pdgCode % 10 * static_cast<unsigned short>(pow(10, i));
            pdgCode /= 10;
        }
        return data;
    }

    inline int AZToPdg(AZ data) {
        return 1000000000 + data.first * 10 + data.second * 10000;
    }


    /* ----------------------- INTERFACES ----------------------- */


    // Filter interface to prevent code duplication
    class VFilter{
    public:
        virtual ~VFilter() = 0;
    };

    inline VFilter::~VFilter() = default;

    /*
     * This is a generator interface. Generators are the first step of the MC simulation: they take data from existing
     * files or encapsulate nucleus-nucleus collision generators.
     */

    class VGenerator : public VFilter{
    public:
        ~VGenerator() override = 0;

        virtual EventData operator()() = 0;
    };

    inline VGenerator::~VGenerator() = default;

    /*
    * This is a converter interface. It is inherited by all filters that are in the middle of MC simulation.
    * Converters are intended to transform data from previous steps using map and reduce methods.
    */

    class VConverter : public VFilter{
    public:
        ~VConverter() override = 0;

        virtual EventData operator()(EventData data) = 0;
    };

    inline VConverter::~VConverter() = default;

    /*
     * This is a writer interface. Writers are what the name suggests - they implement writing results into different
     * data format
     */

    class VWriter : public VFilter {
    public:
        ~VWriter() override = 0;

        virtual void operator()(cola::EventData) = 0;
    };

    inline VWriter::~VWriter() = default;

    /*
     * This is a factory interface. Factories are classes used to create desired instances of filters. This abstraction
     * is required to implement dependency injection. Factories are registered in the metaprocessor class
     */

    class VFactory{
    public:
        virtual ~VFactory();
        virtual VFilter* create() = 0;      //the pointer is passed to uniqEue_ptr constructor, therefore no leaks are possible
    };

    inline VFactory::~VFactory() = default;


    /* ----------------------- MANAGER ----------------------- */


    struct MetaData {
        std::string generatorName;
        std::string converterName;
        std::string writerName;
    };

    struct FilterAnsamble{
        std::unique_ptr<VGenerator> generator;
        std::unique_ptr<VConverter> converter;
        std::unique_ptr<VWriter> writer;
    };

    class MetaProcessor { //TODO: make everything initialise in constructor
    public:
        MetaProcessor() = default;
        ~MetaProcessor() = default;

        void reg(std::shared_ptr<VFactory> factory, const std::string& name, const std::string& type);
        FilterAnsamble parse(const MetaData& data);

    private:
        std::map<std::string, std::shared_ptr<VFactory>> generatorMap;
        std::map<std::string, std::shared_ptr<VFactory>> converterMap;
        std::map<std::string, std::shared_ptr<VFactory>> writerMap;

        void regGen(std::shared_ptr<VFactory>&& factory, const std::string& name){generatorMap.emplace(name, std::move(factory));}
        void regConv(std::shared_ptr<VFactory>&& factory, const std::string& name){converterMap.emplace(name, std::move(factory));}
        void regWrite(std::shared_ptr<VFactory>&& factory, const std::string& name){writerMap.emplace(name, std::move(factory));}
    };

    class ColaRunManager {
    public:
        explicit ColaRunManager(FilterAnsamble&& ansamble) : filterAnsamble(std::move(ansamble)) {}
        ~ColaRunManager() = default;
        void run();
    private:
        FilterAnsamble filterAnsamble;
    };


} //cola

#endif //COLA_COLA_HH
