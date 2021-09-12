//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_APPLICATION_HPP
#define CRYPTOBOT_APPLICATION_HPP
#include "Common.hpp"
#include "../include/systems/DataRetriever.hpp"
#include "../include/components/Data.hpp"
/**
 * Main class of the program. 
 */
class Application : public entityx::EntityX {
public:

    /**
     * Constructor.
     * @param argc Arguments count.
     * @param arguments Program arguments.
     */
    Application(int argc, char** arguments);

    /**
     * Starts the application. 
     */
    void start();

private:
    // Display screen.
    // Use --no-display argument to display nothing on screen.
    bool mDisplay{true};
};
#endif
