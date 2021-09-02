//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_DATAS_HPP
#define CRYPTOBOT_DATAS_HPP
#include "../Common.hpp"
struct TrendingTokenComp
{
    bool operator()(const std::pair<unsigned int, std::string>& lhs, const std::pair<unsigned int, std::string>& rhs) const
    {
        return lhs.first < rhs.first;
    }
};
struct Data
{
    Data():lock(std::make_shared<std::mutex>()){}

    std::map<std::string,std::vector<std::pair<double,double>>> byTokenName;
    std::map<std::string,std::vector<std::pair<double,double>>> linearRegressionByTokenName;
    std::map<std::string,std::vector<std::pair<double,double>>> movingAverageByTokenName;
    std::map<std::string,std::vector<std::pair<double,double>>> supportMaxByTokenName;
    std::map<std::string,std::vector<std::pair<double,double>>> supportMinByTokenName;

    Json::Value binanceTokenPrice;
    std::string geckoTrending;
    Json::Value geckoTrendingJson;

    std::set<std::pair<unsigned int,std::string>,TrendingTokenComp> trendingTokens;

    float refreshTick = 0.f;

    std::shared_ptr<std::mutex> lock;
};
#endif //CRYPTOBOT_DATAS_HPP
