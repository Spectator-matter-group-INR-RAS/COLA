//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_EVENTDATA_HH
#define GENERATORFRAMEWORK_EVENTDATA_HH

namespace cola {

struct EventIniState{
    int a;
};

struct EventParticles{
    int a;
};

class EventData{
public:
    EventIniState iniState;
    EventParticles particles;
};
}

#endif //GENERATORFRAMEWORK_EVENTDATA_HH
