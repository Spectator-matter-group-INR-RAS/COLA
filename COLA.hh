#ifndef COLA_COLA_HH
#define COLA_COLA_HH

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

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

    // initial nuclei and kinematics, diagnostics information
    struct EventIniState{
        int pdgCodeA;
        int pdgCodeB;

        double pZA;
        double pZB;
        double energy;      // per nucleon or sqrtSnn, depends on pZB being zero

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

        EventParticles iniStateParticles;
    };

    struct EventData{
        EventIniState iniState;
        EventParticles particles;
    };

    //A and Z of an ion
    typedef std::pair<unsigned short, unsigned short> AZ;

    //converters
    AZ pdgToAZ (int pdgCode);

    int AZToPdg(AZ data);


    /* ----------------------- INTERFACES ----------------------- */


    // Filter interface to prevent code duplication
    class VFilter{
    public:
        VFilter() = default;
        VFilter (const VFilter&) = delete;
        VFilter (VFilter&&) = delete;
        VFilter& operator=(const VFilter&) = delete;
        VFilter& operator=(VFilter&&) = delete;
        virtual ~VFilter() = 0;
    };

    inline VFilter::~VFilter() = default;

    /*
     * This is a generator interface. Generators are the first step of the MC simulation: they take data from existing
     * files or encapsulate nucleus-nucleus collision generators.
     */

    class VGenerator : public VFilter{
    public:
        VGenerator() = default;
        VGenerator (const VGenerator&) = delete;
        VGenerator (VGenerator&&) = delete;
        VGenerator& operator=(const VGenerator&) = delete;
        VGenerator& operator=(VGenerator&&) = delete;
        ~VGenerator() override = 0;

        virtual std::unique_ptr<EventData> operator()() = 0;
    };

    inline VGenerator::~VGenerator() = default;

    /*
    * This is a converter interface. It is inherited by all filters that are in the middle of MC simulation.
    * Converters are intended to transform data from previous steps using map and reduce methods.
    */

    class VConverter : public VFilter{
    public:
        VConverter() = default;
        VConverter (const VConverter&) = delete;
        VConverter (VConverter&&) = delete;
        VConverter& operator=(const VConverter&) = delete;
        VConverter& operator=(VConverter&&) = delete;
        ~VConverter() override = 0;

        virtual std::unique_ptr<EventData> operator()(std::unique_ptr<EventData>&&) = 0;
    };

    inline VConverter::~VConverter() = default;

    /*
     * This is a writer interface. Writers are what the name suggests - they implement writing results into different
     * data format
     */

    class VWriter : public VFilter {
    public:
        VWriter() = default;
        VWriter (const VWriter&) = delete;
        VWriter (VWriter&&) = delete;
        VWriter& operator=(const VWriter&) = delete;
        VWriter& operator=(VWriter&&) = delete;
        ~VWriter() override = 0;

        virtual void operator()(std::unique_ptr<EventData>&&) = 0;
    };

    inline VWriter::~VWriter() = default;

    class VFactory{
    public:
        VFactory() = default;
        VFactory (const VFactory&) = delete;
        VFactory (VFactory&&) = delete;
        VFactory& operator=(const VFactory&) = delete;
        VFactory& operator=(VFactory&&) = delete;
        virtual ~VFactory() = default;
        virtual VFilter* create(const std::map<std::string, std::string>&) = 0;      //the pointer is passed to unique_ptr constructor, therefore no leaks are possible
    };


    std::unique_ptr<EventData> operator|(const std::unique_ptr<VGenerator>&, const std::unique_ptr<VConverter>&);
    std::unique_ptr<EventData> operator|(std::unique_ptr<EventData>&&, const std::unique_ptr<VConverter>&);
    void operator|(std::unique_ptr<EventData>&&, const std::unique_ptr<VWriter>&);

    enum class FilterType: char {
        generator,
        converter,
        writer
    };

    /* ----------------------- METAPARSER ----------------------- */

    struct FilterEnsemble{
        std::unique_ptr<VGenerator> generator;
        std::vector<std::unique_ptr<VConverter>> converters;
        std::unique_ptr<VWriter> writer;
    };

    class MetaProcessor {
    public:
        MetaProcessor() = default;
        // Preferred constructor, moves pointers to Factories out of filterMap.
        explicit MetaProcessor(std::map<std::string, std::pair<std::unique_ptr<VFactory>, FilterType>>& filterMap);
        ~MetaProcessor() = default;

        void reg(std::unique_ptr<VFactory>&& factory, const std::string& name, FilterType type);
        FilterEnsemble parse(const std::string& fname) const;

    private:
        std::map<std::string, std::unique_ptr<VFactory>> generatorMap;
        std::map<std::string, std::unique_ptr<VFactory>> converterMap;
        std::map<std::string, std::unique_ptr<VFactory>> writerMap;

        void regGen(std::unique_ptr<VFactory>&& factory, const std::string& name){ generatorMap.emplace(name, std::move(factory)); }
        void regConv(std::unique_ptr<VFactory>&& factory, const std::string& name){ converterMap.emplace(name, std::move(factory)); }
        void regWrite(std::unique_ptr<VFactory>&& factory, const std::string& name){ writerMap.emplace(name, std::move(factory)); }
    };

    /*-----------------------MANAGER-------------------------*/

    class ColaRunManager {
    public:
        explicit ColaRunManager(FilterEnsemble&& ensemble) : filterEnsemble(std::move(ensemble)) {}
        ~ColaRunManager() = default;
        void run(int n = 1) const;
    private:
        FilterEnsemble filterEnsemble;
    };

} //cola


#endif //COLA_COLA_HH
