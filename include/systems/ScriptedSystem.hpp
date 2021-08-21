//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_SCRIPTEDSYSTEM_HPP
#define CRYPTOBOT_SCRIPTEDSYSTEM_HPP
#include "../Common.hpp"
#include "ThreadedSystem.hpp"
#include "../components/Data.hpp"
template<class SUBSYSTEM>
class ScriptedSystem : public ThreadedSystem<SUBSYSTEM>
{
    using ThreadedSystem<SUBSYSTEM>::mThreadRunning;
public:
    explicit ScriptedSystem(const std::string& pDirectoryName = "default") : mDirectoryName(pDirectoryName){}
    void configure(entityx::EntityManager &es, entityx::EventManager &events)
    {
        mChai.add(chaiscript::var(&es),"cryptobot");

        mChai.add(chaiscript::fun(&entityx::EntityManager::each<Data>),"forEachDataset");

        mChai.eval_file("scripts/"+mDirectoryName+"/configure.chai");
    }
protected:
    void run(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
    {
        mThreadRunning = true;
        mChai.eval_file("scripts/"+mDirectoryName+"/update.chai");
        mThreadRunning = false;
    }
    bool shouldRun()
    {
        return true;
    }
    bool shouldRunOnThread()
    {
        return true;
    }
    chaiscript::ChaiScript mChai;
    std::string mDirectoryName;
    float mCumulDeltaT=0.f;

};

#endif //CRYPTOBOT_SCRIPTEDSYSTEM_HPP
