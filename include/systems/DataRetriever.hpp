//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_DATARETRIEVER_HPP
#define CRYPTOBOT_DATARETRIEVER_HPP
#include "../Common.hpp"
#include "ThreadedSystem.hpp"
#include "../Utils.hpp"
#include "../components/Data.hpp"
class DataRetriever : public ThreadedSystem<DataRetriever>
{
    protected:
        void run(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt);
        bool shouldRun(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt);
        bool shouldRunOnThread();
    private:
        binance::Server mBinanceServer;
};
#endif //CRYPTOBOT_DATARETRIEVER_HPP
