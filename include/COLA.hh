#ifndef COLA_COLA_HH
#define COLA_COLA_HH

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include <boost/algorithm/string.hpp>

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

    //initial nuclei nad kinematics, diagnostics information
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

        EventParticles iniStateParticles;
    };

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

        virtual std::unique_ptr<EventData> operator()() = 0;
    };

    inline VGenerator::~VGenerator() = default;

    /*
    * This is a converter interface. It is inherited by all filters that are in the middle of MC simulation.
    * Converters are intended to transform data from previous steps using map and reduce methods.
    */

    class VConverter : public VFilter{
    public:
        ~VConverter() override = 0;

        virtual std::unique_ptr<EventData> operator()(std::unique_ptr<EventData>) = 0;
    };

    inline VConverter::~VConverter() = default;

    /*
     * This is a writer interface. Writers are what the name suggests - they implement writing results into different
     * data format
     */

    class VWriter : public VFilter {
    public:
        ~VWriter() override = 0;

        virtual void operator()(std::unique_ptr<EventData>) = 0;
    };

    inline VWriter::~VWriter() = default;

    class VFactory{
    public:
        virtual ~VFactory();
        virtual VFilter* create(const std::string) = 0;      //the pointer is passed to uniqEue_ptr constructor, therefore no leaks are possible
    };

    inline VFactory::~VFactory() = default;


    inline std::unique_ptr<cola::EventData> operator|(std::shared_ptr<cola::VGenerator> generator, std::shared_ptr<cola::VConverter> converter){
        return (*converter)((*generator)());
    }
    inline std::unique_ptr<cola::EventData> operator|(std::unique_ptr<EventData> data, std::shared_ptr<cola::VConverter> converter){
        return (*converter)(std::move(data));
    }
    inline void operator|(std::unique_ptr<EventData> data, std::shared_ptr<cola::VWriter> writer){
        (*writer)(std::move(data));
    }

    /* ----------------------- METAPARSER ----------------------- */

    struct MetaData {
        std::string generatorName;
        std::queue<std::string> converterNames;
        std::string writerName;
        std::map<std::string, std::string> filterParamMap;
    };

    struct FilterAnsamble{
        std::unique_ptr<VGenerator> generator;
        std::vector<std::unique_ptr<VConverter>> converters;
        std::unique_ptr<VWriter> writer;
    };

    class MetaProcessor { //TODO: make everything initialise in constructor
    public:
        MetaProcessor() = default;
        ~MetaProcessor() = default;

        void reg(std::unique_ptr<VFactory>&&     factory, const std::string& name, const std::string& type);
        FilterAnsamble parse(const std::string data);

    private:
        std::map<std::string, std::unique_ptr<VFactory>> generatorMap;
        std::map<std::string, std::unique_ptr<VFactory>> converterMap;
        std::map<std::string, std::unique_ptr<VFactory>> writerMap;

        void regGen(std::unique_ptr<VFactory>&& factory, const std::string& name){ generatorMap.emplace(name, std::move(factory)); }
        void regConv(std::unique_ptr<VFactory>&& factory, const std::string& name){ converterMap.emplace(name, std::move(factory)); }
        void regWrite(std::unique_ptr<VFactory>&& factory, const std::string& name){ writerMap.emplace(name, std::move(factory)); }

        MetaData parseStrToMeta(const std::string data);
    };

    inline cola::FilterAnsamble cola::MetaProcessor::parse(std::string data) {
        FilterAnsamble ansamble;
        MetaData meta = parseStrToMeta(data);
        ansamble.generator = std::unique_ptr<VGenerator>(dynamic_cast<VGenerator*>(generatorMap.at(meta.generatorName)->create(meta.filterParamMap.at(meta.generatorName))));
        while(!meta.converterNames.empty())
        {ansamble.converters.push_back(std::unique_ptr<VConverter>(dynamic_cast<VConverter*>(converterMap.at(meta.converterNames.front())->create(meta.filterParamMap.at(meta.converterNames.front())))));
            meta.converterNames.pop();}
        ansamble.writer = std::unique_ptr<VWriter>(dynamic_cast<VWriter*>(writerMap.at(meta.writerName)->create(meta.filterParamMap.at(meta.writerName))));
        return ansamble;
    }

    inline cola::MetaData cola::MetaProcessor::parseStrToMeta(const std::string data) {
        std::vector<std::string> filters; 
        cola::MetaData metaData;
        std::string trimedData = boost::trim_copy_if(data, boost::is_any_of("\n"));
        boost::split(filters, trimedData, boost::is_any_of("\n"));
        for(int flt = 0; flt < filters.size(); ++flt){
            std::vector<std::string> tmp;
            boost::split(tmp, filters.at(flt), boost::is_any_of(" "));
            auto res = boost::trim_left_copy_if(filters.at(flt), boost::is_any_of(tmp.at(0)+" "));
            metaData.filterParamMap.emplace(tmp.at(0), res);
            if (flt == 0) {
                metaData.generatorName = tmp.at(0);
            } else if (flt == filters.size() - 1) {
                metaData.writerName = tmp.at(0);
            } else {
                metaData.converterNames.push(tmp.at(0));
            }
        }
        return metaData;
    }
    
    inline void cola::MetaProcessor::reg(std::unique_ptr<VFactory>&& factory, const std::string& name, const std::string& type) {
        if (type == "generator") { regGen(std::move(factory), name); }
        else if (type == "converter") { regConv(std::move(factory), name); }
        else if (type == "writer") { regWrite(std::move(factory), name); }
        else {std::cerr<<"ERROR in MetaProcessor: No such type of filter"; }
    }

    /*-----------------------MANAGER-------------------------*/

    class ColaRunManager {
    public:
        explicit ColaRunManager(FilterAnsamble&& ansamble) : filterAnsamble(std::move(ansamble)) {}
        ~ColaRunManager() = default;
        void run(int n = 1);
        void test();
    private:
        FilterAnsamble filterAnsamble;
    };

    inline void cola::ColaRunManager::run(int n) {
        for(int k = 0; k < n; k++) {
            auto event = (*(filterAnsamble.generator))();
            for (const auto& converter : filterAnsamble.converters)
                event = (*converter)(std::move(event));
            (*(filterAnsamble.writer))(std::move(event));
        }
    }

} //cola


#endif //COLA_COLA_HH
