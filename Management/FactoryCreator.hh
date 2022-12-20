//
// Created by alexsvetlichnyy on 20.12.22.
//

#ifndef GENERATORFRAMEWORK_FACTORYCREATOR_HH
#define GENERATORFRAMEWORK_FACTORYCREATOR_HH

#include <memory>

namespace cola {
    template<class TImpl>
    struct AbstractFactory {
        virtual std::shared_ptr<TImpl> create() const = 0;
    };

    template<class T>
    struct Factory : AbstractFactory<T> {
        std::shared_ptr<T> create() { return std::make_shared<T>(new T()); }
    };

    template<class T>
    struct DepInjection {
        std::shared_ptr<Factory<T>> makeFactory() { return std::make_shared<Factory<T>>(new Factory<T>); }
    };
}
#endif //GENERATORFRAMEWORK_FACTORYCREATOR_HH
