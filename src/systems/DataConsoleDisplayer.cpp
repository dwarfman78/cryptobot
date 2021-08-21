//
// Created by dwarf on 17/07/2021.
//
#include "../../include/systems/DataConsoleDisplayer.hpp"
void DataConsoleDisplayer::update(entityx::EntityManager &es, entityx::EventManager &events, entityx::TimeDelta dt)
{
   /* es.each<Data>([&](entityx::Entity entity, Data& data) {
        if(mCumulDeltaT<data.refreshTick) {
            for (auto &byTokenName : data.byTokenName) {
                auto &historic = byTokenName.second;
                //auto reg = regression(historic);

                //if(std::abs(reg.first)>0.0001f)
                //{
                auto &displaySymbol = byTokenName.first;

                if (displaySymbol == "BTCUSDT") {
                    std::cout << displaySymbol << std::endl;
                    //std::cout << "linear regression : y = " << reg.first << "x + " << reg.second << std::endl;
                    for (auto &pairValue : historic) {
                        std::cout << "\t" << pairValue.first << " - " << pairValue.second << std::endl;
                    }
                    //graph(historic);
                }
                //}
            }
            mCumulDeltaT = data.refreshTick;
        }
    });*/
}
