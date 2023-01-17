#ifndef GENERATORFRAMEWORK_VWRITER_HH
#define GENERATORFRAMEWORK_VWRITER_HH

#include "EventData.hh"
#include "VFilter.hh"

/*
 * This is a writer interface. Writers are what the name suggests - they implement writing results into different
 * data format
 */

namespace cola {

    class VWriter : public VFilter {
    public:
        ~VWriter() override = 0;

        virtual void operator()(cola::EventData) = 0;
    };

    inline VWriter::~VWriter() = default;

} //cola

#endif //GENERATORFRAMEWORK_VWRITER_HH
