//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_DATACONSOLEDISPLAYER_HPP
#define CRYPTOBOT_DATACONSOLEDISPLAYER_HPP
#include "../Common.hpp"
#include "../components/Data.hpp"
class DataConsoleDisplayer : public entityx::System<DataConsoleDisplayer>
{
public:
    void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt);
private:
    float mCumulDeltaT=0.f;
};
#endif //CRYPTOBOT_DATACONSOLEDISPLAYER_HPP
