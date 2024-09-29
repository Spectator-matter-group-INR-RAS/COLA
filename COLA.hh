#ifndef COLA_COLA_HH
#define COLA_COLA_HH

#include <cmath>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include "LorentzVector.hh"

namespace cola {
    using LorentzVector = LorentzVectorImpl<double>;

    /** A typedef representing mass and charge of a nucleon.
     */
    using AZ = std::pair<unsigned short, unsigned short>;

    /** PDG code to AZ converter.
     *  **WARNING:** this function is intended to process heavy ions PDG codes.
     *  @param pdgCode PDG code of the ion.
     *  @return AZ of the ion.
     */
    AZ pdgToAZ(int pdgCode);

    /** AZ to PDG code convverter.
     *  @param data AZ of the ion.
     *  @return PDG code of the ion
     */
    int AZToPdg(AZ data);

    /** \defgroup Data Data Classes and supporting methods.
     * @{
     */

    /** Particle class by generator output.
     *  This enum represents various outcomes of a generator event for every particle.
     */
    enum class ParticleClass: char {
        produced,     /**< A particle that was not present in the starting nuclei. */
        elasticA,     /**< A particle that was present in the projectile nucleus and has experienced only elastic interactions. */
        elasticB,     /**< A particle that was present in the target nucleus and has experienced only elastic interactions. */
        nonelasticA,  /**< A particle that was present in the projectile nucleus and has experienced at least one non-elastic interaction. */
        nonelasticB,  /**< A particle that was present in the target nucleus and has experienced at least one non-elastic interaction. */
        spectatorA,   /**< A particle that was present in the projectile nucleus and hasn't experienced any interactions. */
        spectatorB    /**< A particle that was present in the projectile nucleus and hasn't experienced any interactions. */
    };

    /** Particle data.
     *  A structure representing data about a single particle
     */
    struct Particle {
        AZ getAZ() const;

        LorentzVector position; /**< Position <t, x, y, z> vector. */

        LorentzVector momentum; /**< Momentum <e, x, y, z> vector. */

        int pdgCode;    /**< PDG code of the particle. */
        ParticleClass pClass;   /**< Data about particle origin. See ParticleClass for more info.*/
    };

    /**
     * Convenient typedef for Particle vector.
     */
    using EventParticles = std::vector<Particle>;

    /** Initial state data.
     *  This structure contains data about initial state of any given event.
     */
    struct EventIniState {
        int pdgCodeA;           /**< PDF code of the projectile. */
        int pdgCodeB;           /**< PDF code of the target. */

        double pZA;             /** Axial momentum of the projectile */
        double pZB;             /** Axial momentum of the target */
        double energy;          /** Incidental energy of the event. Depending on pZB being zero, this is either \f$E/A\f$ of target or \f$\sqrt{s_{NN}}\f$. */

        float sectNN;           /** Nucleon-Nucleon cross section from generator. */
        float b;                /** Impact parameter of the event. */

        int nColl;              /** Diagnostic. Total number of collisions. */
        int nCollPP;            /** Diagnostic. Number of proton-proton. */
        int nCollPN;            /** Diagnostic. Number of proton-neutron collisions. */
        int nCollNN;            /** Diagnostic. Number of neutron-neutron collisions. */
        int nPart;              /** Diagnostic. Total number of participants. */
        int nPartA;             /** Diagnostic. Number of participants from the projectile nucleus. */
        int nPartB;             /** Diagnostic. Number of participants from the target nucleus. */

        float phiRotA;          /** Diagnostic. Polar angle \f$\phi\f$ of rotation of the projectile nucleon. */
        float thetaRotA;        /** Diagnostic. Polar angle \f&\Theta\f$ of rotation of the projectile nucleon. */
        float phiRotB;          /** Diagnostic. Polar angle \f$\phi\f$ of rotation of the target nucleon. */
        float thetaRotB;        /** Diagnostic. Polar angle \f$\Theta\f$ of rotation of the target nucleon. */

        EventParticles iniStateParticles;   /** The array of all Particles just before the event. */
    };

    /** A structure combining EventIniState and EventParticles of the event.
     */
    struct EventData {
        EventIniState iniState;
        EventParticles particles;
    };

    /** @}
     * \defgroup Interface Pure abstract classes used for dependency injection.
     * @{
     */

    /** A common abstract parent class representing any model in the pipeline.
     *  A single model in the COLA-driven pipeline is named a Filter.
     */
    class VFilter {
    public:
        VFilter() = default;
        VFilter (const VFilter&) = delete;
        VFilter (VFilter&&) = delete;
        VFilter& operator=(const VFilter&) = delete;
        VFilter& operator=(VFilter&&) = delete;
        virtual ~VFilter() = 0;
    };

    inline VFilter::~VFilter() = default;

    /** Generator abstract class.
     *  This is a generator interface. Generators are the first step of the MC simulation: they take data from existing
     *  files or encapsulate nucleus-nucleus collision generators.
     */
    class VGenerator : public VFilter {
    public:
        VGenerator() = default;
        VGenerator (const VGenerator&) = delete;
        VGenerator (VGenerator&&) = delete;
        VGenerator& operator=(const VGenerator&) = delete;
        VGenerator& operator=(VGenerator&&) = delete;
        ~VGenerator() override = 0;

        /** A method to request one event from the generator.
         *  Users are supposed to override this method to provide a single event from the generator model.
         *  @return A pointer to the EventData of the produced event.
         */
        virtual std::unique_ptr<EventData> operator()() = 0;
    };

    inline VGenerator::~VGenerator() = default;

    /** Converter abstract class.
    *  This is a converter interface. It is inherited by all filters that are in the middle of MC simulation.
    *  Converters are intended to transform data from previous steps.
    */
    class VConverter : public VFilter {
    public:
        VConverter() = default;
        VConverter (const VConverter&) = delete;
        VConverter (VConverter&&) = delete;
        VConverter& operator=(const VConverter&) = delete;
        VConverter& operator=(VConverter&&) = delete;
        ~VConverter() override = 0;

        /** A method to process one event by the converter model.
         *  Users are supposed to override this method to process a single event by the converter model.
         *  @param data A pointer to the EventData to be processed.
         *  @return A pointer to the EventData of the processed event.
         */
        virtual std::unique_ptr<EventData> operator()(std::unique_ptr<EventData>&& data) = 0;
    };

    inline VConverter::~VConverter() = default;

    /** Writer abstract class.
     *  This is a writer interface. Writers are what the name suggests - they implement writing results into different
     *  data formats. Note, that it isn't necessary to *only* write events to memory with this class. If you need to
     *  process data in a way that the EventData is incompatible with the output (for example, you want to model a
     *  detector response to the generated event) you can encapsulate these calculations in a writer class.
     */
    class VWriter : public VFilter {
    public:
        VWriter() = default;
        VWriter (const VWriter&) = delete;
        VWriter (VWriter&&) = delete;
        VWriter& operator=(const VWriter&) = delete;
        VWriter& operator=(VWriter&&) = delete;
        ~VWriter() override = 0;

        /** A method to save one event by the writer.
         *  Users are supposed to override this method to save a single event. Note, that the implementation is left
         *  to the user: you can save events to buffer with this method and write them to the filesystem in batches if
         *  you please so.
         *  @param data A pointer to the EventData to be saved.
         */
        virtual void operator()(std::unique_ptr<EventData>&& data) = 0;
    };

    inline VWriter::~VWriter() = default;

    /** Factory abstract class.
     * This is a factory interface. It generates a Filter with its VFactory::create method. DI in COLA works via using the
     * factory classes, which are registered in a MetaProcessor instance.
     */
    class VFactory {
    public:
        VFactory() = default;
        VFactory (const VFactory&) = delete;
        VFactory (VFactory&&) = delete;
        VFactory& operator=(const VFactory&) = delete;
        VFactory& operator=(VFactory&&) = delete;
        virtual ~VFactory() = default;

        /** The method for constructing filters.
         *  This method is intended to be called in MetaProcessor with key-value pairs obtained from a XML-file.
         *  @param metaData A dictionary with key-value pairs needed for configuring a model.
         *  @return A configured class that is a VFilter child.
         */
        virtual VFilter* create(const std::map<std::string, std::string>& metaData) = 0;
    };


    std::unique_ptr<EventData> operator|(const std::unique_ptr<VGenerator>&, const std::unique_ptr<VConverter>&);
    std::unique_ptr<EventData> operator|(std::unique_ptr<EventData>&&, const std::unique_ptr<VConverter>&);
    void operator|(std::unique_ptr<EventData>&&, const std::unique_ptr<VWriter>&);

    /** An enum for marking Filter types.
     */
    enum class FilterType: char {
        generator,
        converter,
        writer
    };
    /** @}
     *  \defgroup Metadata Classes for constructing and running a model.
     *  @{
     */

    /** A structure representing the model pipeline.
     */
    struct FilterEnsemble {
        std::unique_ptr<VGenerator> generator;                  /**< Event generator. */
        std::vector<std::unique_ptr<VConverter>> converters;    /**< Vector of converters, applied step-by-step. */
        std::unique_ptr<VWriter> writer;                        /**< Writer to save the results. */
    };

    /** A class for processing meta information.
     *  This class stores data about all available Filters and corresponding factory classes and uses it to create the
     *  pipeline using its MetaProcessor:parse method to read all needed information from an XML-file.
     */
    class MetaProcessor {
    public:

        /** Default constructor.
         */
        MetaProcessor() = default;

        /** Constructor with immediate factories registration.
         * Note that unique pointers in the dictionary are invalidated.
         * @param filterMap A dictionary with all relevant information. Note that unique pointers in the dictionary are invalidated.
         */
        explicit MetaProcessor(std::map<std::string, std::pair<std::unique_ptr<VFactory>, FilterType>>& filterMap);

        ~MetaProcessor() = default;

        /** A method for registering new Filters.
         * @param factory A rvalue-reference to the Filter factory pointer
         * @param name The name of the Filter.
         * @param type The type of the Filter. See FilterType.
         */
        void reg(std::unique_ptr<VFactory>&& factory, const std::string& name, FilterType type);

        /** A method to parse a XML-file to set up a configured FilterEnsemble.
         *  This method opens an XML-file @param fName to get the information to set up the model.
         *  Inside the root element should be one <generator> element followed by any number of <converter> elements
         *  (none is possible) and, finally, a <writer> element. Each element must have "name" attribute followed by
         *  any number of additional attributes. These attributes are then passed to the corresponding factory's VFactory::create
         *  method as a dictionary with keys being attribute names and values - attribute values.
         *  This method throws an error if a relevant factory isn't found or XML-file is malformed.
         *  @param fName Name with the configuration XML-file.
         *  @return A configured FilterEnsemble.
         */
        FilterEnsemble parse(const std::string& fName) const;

    private:
        std::map<std::string, std::unique_ptr<VFactory>> generatorMap;
        std::map<std::string, std::unique_ptr<VFactory>> converterMap;
        std::map<std::string, std::unique_ptr<VFactory>> writerMap;

        void regGen(std::unique_ptr<VFactory>&& factory, const std::string& name){ generatorMap.emplace(name, std::move(factory)); }
        void regConv(std::unique_ptr<VFactory>&& factory, const std::string& name){ converterMap.emplace(name, std::move(factory)); }
        void regWrite(std::unique_ptr<VFactory>&& factory, const std::string& name){ writerMap.emplace(name, std::move(factory)); }
    };

    /** Manager class.
     * Currently more of a boilerplate, but potentially useful to incorporate parallel computing.
     */
    class ColaRunManager {
    public:
        ColaRunManager() = delete;
        /** A constructor that moves the configured FilterEnsemble into the manager.
         * @param ensemble Configured model.
         */
        explicit ColaRunManager(FilterEnsemble&& ensemble) : filterEnsemble(std::move(ensemble)) {}
        ~ColaRunManager() = default;
        /** A method to run the resulting model @param n times.
         * @param n Number of runs.
         */
        void run(int n = 1) const;
    private:
        FilterEnsemble filterEnsemble;
    };
} // cola

#endif //COLA_COLA_HH
