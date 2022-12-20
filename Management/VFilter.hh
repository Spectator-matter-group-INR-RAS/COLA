#ifndef GENERATORFRAMEWORK_VFILTER_HH
#define GENERATORFRAMEWORK_VFILTER_HH

// Filter interface to prevent code duplication

namespace cola {

    class VFilter{
    public:
        virtual ~VFilter() = 0;
    };

    inline VFilter::~VFilter() = default;

} //cola

#endif //GENERATORFRAMEWORK_VFILTER_HH
