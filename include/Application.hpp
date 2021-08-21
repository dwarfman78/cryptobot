//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_APPLICATION_HPP
#define CRYPTOBOT_APPLICATION_HPP
#include "Common.hpp"
#include "../include/systems/DataRetriever.hpp"
#include "../include/components/Data.hpp"
class Application : public entityx::EntityX {
public:

    Application();
    void start();
};
#endif
