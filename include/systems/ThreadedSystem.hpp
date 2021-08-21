//
// Created by dwarf on 11/08/2021.
//

#ifndef CRYPTOBOT_THREADEDSYSTEM_HPP
#define CRYPTOBOT_THREADEDSYSTEM_HPP
#include "../Common.hpp"
#include "../components/Data.hpp"
template<class SUBSYSTEM>
class ThreadedSystem : public entityx::System<SUBSYSTEM>
{
public:
    ThreadedSystem() = default;
    ~ThreadedSystem()
    {
        if(mThread.joinable())
        {
            mThread.join();
        }
    }
    void update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)
    {
        if(!mThreadRunning)
        {
            if(mThread.joinable())
                mThread.join();

            auto fdt = static_cast<float>(dt);
            mCumulDeltaT+=fdt;

            if(shouldRun(es,events,dt))
            {
                mLastRun = mCumulDeltaT;
                if(shouldRunOnThread())
                {
                    mThread = std::thread(&ThreadedSystem::run, this, std::ref(es), std::ref(events), dt);
                }
                else
                {
                    run(es,events,dt);
                }
            }
        }
    }
protected:
    virtual void run(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) = 0;
    virtual bool shouldRun(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) = 0;
    virtual bool shouldRunOnThread() = 0;
    float mCumulDeltaT{0.f};
    float mLastRun{0.f};
    std::thread mThread;
    bool mThreadRunning{false};
};
#endif //CRYPTOBOT_THREADEDSYSTEM_HPP
