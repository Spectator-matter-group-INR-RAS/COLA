//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_VFILTER_HH
#define GENERATORFRAMEWORK_VFILTER_HH

class VFilter{
public:
    virtual ~VFilter() = 0;
};
inline VFilter::~VFilter() = default;

#endif //GENERATORFRAMEWORK_VFILTER_HH
