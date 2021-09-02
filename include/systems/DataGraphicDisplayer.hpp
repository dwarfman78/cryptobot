//
// Created by dwarf on 18/07/2021.
//

#ifndef CRYPTOBOT_DATAGRAPHICDISPLAYER_HPP
#define CRYPTOBOT_DATAGRAPHICDISPLAYER_HPP
#include "../Common.hpp"
#include "../components/Data.hpp"
#include "../components/GuiEvent.hpp"
class DataGraphicDisplayer : public entityx::System<DataGraphicDisplayer>
{
public:
    void configure(entityx::EntityManager &es, entityx::EventManager &events);
    void update(entityx::EntityManager &es, entityx::EventManager &events,entityx::TimeDelta dt);
private:
    float mCumulDeltaT=0.f;

    chaiscript::ChaiScript mChai;

    void displayGraphs(entityx::EntityManager &es);

    void displayKeywords(entityx::EntityManager &es,entityx::EventManager& em);

    void displayDataTable(entityx::EntityManager &es);

    void displayOptions(entityx::EntityManager &es, entityx::EventManager &events) const;

    void displaySearch(entityx::EntityManager &entityManager, entityx::EventManager &events);

    void displayLinearRegression(std::vector<double>& vectorX, std::pair<double,double>& pairAB, unsigned int offset, unsigned int vectorPriceSize);

    static bool mFilterByKeyWord;

    static bool mShowOnlyAccPos;

    static bool mCalculateLinearRegression;

    static bool mCalculateMinAndMax;

    std::vector<std::tuple<std::string,double,double,ImVec4,ImVec4>> mDisplayVector;

    std::map<std::string,std::vector<std::pair<double,double>>> mDisplayByTokenName;

    Json::Value mGeckoTrendingJson;

    std::set<std::pair<unsigned int,std::string>,TrendingTokenComp> mTrendingTokens;

    static std::string mRegexpFilter;
};
#endif //CRYPTOBOT_DATAGRAPHICDISPLAYER_HPP
