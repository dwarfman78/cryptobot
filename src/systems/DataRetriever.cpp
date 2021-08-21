//
// Created by dwarf on 17/07/2021.
//

#include "../../include/systems/DataRetriever.hpp"
bool DataRetriever::shouldRun(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    return mCumulDeltaT - mLastRun > 5000;
}
bool DataRetriever::shouldRunOnThread()
{
    return true;
}
void DataRetriever::run(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    mThreadRunning = true;
    es.each<Data>([&](entityx::Entity entity, Data& data) {

            binance::Market market(mBinanceServer);

            RestClient::init();
            RestClient::Connection conn("https://api.coingecko.com/api/v3");

            std::cout << "Call Gecko API" << std::endl;
            RestClient::Response response = conn.get("/search/trending");

            std::cout << "Call of binance API" << std::endl;
            Json::Value tempPrices;
            market.getAllPrices(tempPrices);

            std::lock_guard<std::mutex> guard(*data.lock);
            data.binanceTokenPrice = tempPrices;
            data.geckoTrending = response.body;
            data.refreshTick = mCumulDeltaT;

    });
    mThreadRunning = false;
}