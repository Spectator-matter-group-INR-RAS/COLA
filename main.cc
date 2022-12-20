#include <iostream>
#include "VFactory.hh"
int main() {
    auto intFact = cola::DInjection<int>().makeFactory();
}
