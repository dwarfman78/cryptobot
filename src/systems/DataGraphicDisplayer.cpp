//
// Created by dwarf on 18/07/2021.
//
#include "../../include/systems/DataGraphicDisplayer.hpp"
#include "../../include/Utils.hpp"
bool DataGraphicDisplayer::mFilterByKeyWord = false;
bool DataGraphicDisplayer::mShowOnlyAccPos = false;
bool DataGraphicDisplayer::mCalculateLinearRegression = false;
bool DataGraphicDisplayer::mCalculateMinAndMax = false;
std::string DataGraphicDisplayer::mRegexpFilter = "";

const ImVec4 GREEN{0,0.5f,0,1.0f};
const ImVec4 RED{0.5f,0,0,1.0f};

void DataGraphicDisplayer::configure(entityx::EntityManager &es, entityx::EventManager &events)
{
    mChai.add(chaiscript::bootstrap::standard_library::map_type<std::map<std::string, std::string> >("ConfigMap"));
    mChai.add(chaiscript::var(&Utils::config), "config");
    mChai.eval_file("scripts/config.chai");
}

void DataGraphicDisplayer::update(entityx::EntityManager &es, entityx::EventManager &events,entityx::TimeDelta dt)
{
    displaySearch(es,events);
    displayKeywords(es,events);
    displayGraphs(es);
    displayDataTable(es);
    displayOptions(es,events);
}
void DataGraphicDisplayer::displayOptions(entityx::EntityManager &es, entityx::EventManager &events) const
{
    ImGui::Begin("Options");
    ImGui::Checkbox("Display only token with +Acc", &mShowOnlyAccPos);
    if(ImGui::Checkbox("Calculate linear regression", &mCalculateLinearRegression))
    {
        events.emit<CalculateLinearRegressionEvent>(mCalculateLinearRegression);
    }
    if(ImGui::Checkbox("Calculate min and max", &mCalculateMinAndMax))
    {
        events.emit<CalculateMinMaxEvent>(mCalculateMinAndMax);
    }
    ImGui::End();
}
void DataGraphicDisplayer::displayDataTable(entityx::EntityManager &es)
{
    ImGui::Begin("Data");
    if (ImGui::BeginTable("data", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        std::string titles[3] = {"Token","Price","Acc"};
        for (auto& title : titles)
        {
            ImGui::TableNextColumn();
            ImGui::Text(title.c_str());
        }
        es.each<Data>([&](entityx::Entity entity, Data& data) {

            if(data.lock->try_lock()) {
                mDisplayVector.clear();
                for (auto &tokenWithHistory : data.byTokenName) {

                    std::string tokenWithoutAgainst = tokenWithHistory.first;
                    Utils::eraseSubstring(tokenWithoutAgainst, Utils::config["tradingagainst"]);

                    auto vectorPrice = tokenWithHistory.second;
                    double acc{0.0f};
                    auto& linearReg = data.linearRegressionByTokenName[tokenWithHistory.first];
                    
                    if(!linearReg.empty())
                    {
                        if(mCalculateLinearRegression)
                        {
                            auto &pairAB = linearReg.back();
                            acc = pairAB.first;
                        }
                    }

                    double currentPrice = vectorPrice.back().second;
                    double oldPrice = (*(vectorPrice.end() - 2)).second;
                    

                    mDisplayVector.push_back(std::make_tuple(
                            tokenWithoutAgainst,
                            currentPrice,
                            acc,
                            currentPrice > oldPrice ? GREEN : RED,
                            acc > 0 ? GREEN : RED));

                }

                std::sort(mDisplayVector.begin(), mDisplayVector.end(), [](auto elt1, auto elt2) {
                    double acc1 = std::get<2>(elt1);
                    double acc2 = std::get<2>(elt2);

                    return acc1 > acc2;
                });
                data.lock->unlock();
            }

            for(auto& displayTuple : mDisplayVector) {

                std::string tokenWithoutAgainst = std::get<0>(displayTuple);

                if(Utils::isTokenSelected(mRegexpFilter,mFilterByKeyWord,tokenWithoutAgainst,mTrendingTokens))
                {

                    double acc = std::get<2>(displayTuple);

                    if(!mShowOnlyAccPos || mShowOnlyAccPos && acc > 0)
                    {
                        ImGui::TableNextColumn();
                        ImGui::Text("%s", tokenWithoutAgainst.c_str());

                        ImGui::TableNextColumn();
                        double currentPrice = std::get<1>(displayTuple);

                        ImGui::PushStyleColor(ImGuiCol_Text, std::get<3>(displayTuple));
                        ImGui::Text("%s", std::to_string(currentPrice).c_str());
                        ImGui::PopStyleColor();

                        ImGui::TableNextColumn();
                        ImGui::PushStyleColor(ImGuiCol_Text, std::get<4>(displayTuple));
                        ImGui::Text("%s", std::to_string(acc).c_str());
                        ImGui::PopStyleColor();
                    }
                }
            }

        });
        ImGui::EndTable();
    }
    ImGui::End();
}
void DataGraphicDisplayer::displayKeywords(entityx::EntityManager& es,entityx::EventManager& em)
{
    ImGui::Begin("Keywords");
    if(ImGui::Checkbox("Filter by keywords", &mFilterByKeyWord))
    {
        em.emit<FilterByKeywordClickedEvent>(mFilterByKeyWord);
    }

    es.each<Data>([&](entityx::Entity entity, Data& data) {
        if(data.lock->try_lock())
        {
            if(!data.geckoTrending.empty()&&!data.geckoTrendingJson.empty())
            {
                mGeckoTrendingJson = data.geckoTrendingJson;
                mTrendingTokens = data.trendingTokens;
            }
            data.lock->unlock();
        }

        auto coins = mGeckoTrendingJson["coins"];
        for (int i = 0; i < coins.size(); i++)
        {
            ImGui::Text("%s", coins[i]["item"]["symbol"].asCString());
        }
    });
    ImGui::End();
}
void DataGraphicDisplayer::displayGraphs(entityx::EntityManager& es)
{
    ImGui::Begin("Graphs");
    es.each<Data>([&](entityx::Entity entity, Data& data) {

        if(data.lock->try_lock())
        {
            mDisplayByTokenName = data.byTokenName;
            data.lock->unlock();
        }
        for(auto& tokenWithHistory : mDisplayByTokenName) {

            std::string tokenWithoutAgainst = tokenWithHistory.first;
            Utils::eraseSubstring(tokenWithoutAgainst,Utils::config["tradingagainst"]);

            if(Utils::isTokenSelected(mRegexpFilter,mFilterByKeyWord,tokenWithoutAgainst,mTrendingTokens)) 
            {
                auto& vectorPrice = tokenWithHistory.second;

                const unsigned int maxSize = std::stoi(Utils::config["maxdisplayedvalues"]);
                int offset = vectorPrice.size() - maxSize;
                if(offset<0)
                {
                    offset=0;
                }

                std::vector<double> vectorX, vectorY;
                Utils::splitDataInTwoVectors(vectorPrice,vectorX,vectorY,offset,vectorPrice.size()-offset);
                unsigned int vSize = vectorX.size();

                auto tokenName = tokenWithHistory.first.c_str();
                std::pair<double,double> pairAB{0.0f,0.0f};

                if(!data.linearRegressionByTokenName[tokenName].empty())
                {
                    pairAB = data.linearRegressionByTokenName[tokenName].back();
                }
                if(!mShowOnlyAccPos||mShowOnlyAccPos&&pairAB.first > 0) 
                {
                    double yMaxValue = *std::max_element(vectorY.begin(),vectorY.end());
                    ImPlot::SetNextPlotLimits(offset, vSize+2, yMaxValue - (yMaxValue*0.2f), yMaxValue + (yMaxValue*0.2f));

                    if (ImPlot::BeginPlot(tokenName)) 
                    {

                        ImPlot::PlotScatter("Price", &vectorX[0], &vectorY[0], vSize);

                        if(mCalculateLinearRegression)
                        {
                            displayLinearRegression(vectorX,pairAB);

                            auto& hisLinearRegressions = data.hisLinearRegression[tokenName];
                            auto& lowsLinearRegressions = data.lowsLinearRegression[tokenName];
                            
                            if(hisLinearRegressions.size()>0&&lowsLinearRegressions.size()>0)
                            {
                                auto& pairABHis = hisLinearRegressions.back();
                                auto& pairABLows = lowsLinearRegressions.back();
                                
                                displayLinearRegression(vectorX,pairABHis);
                                displayLinearRegression(vectorX,pairABLows);
                            }
                        }

                        if(mCalculateMinAndMax)
                        {
                            std::vector<double> yMaxSupport;
                            yMaxSupport.resize(vSize);
                            if(!data.supportMaxByTokenName[tokenName].empty())
                            {
                                auto &pairABSupportMax = data.supportMaxByTokenName[tokenName].back();
                                std::string plotNameMax{
                                        "MAX(Y = " + std::to_string(pairABSupportMax.first) + " X + " +
                                        std::to_string(pairABSupportMax.second) +
                                        ")"};
                                std::transform(vectorX.begin(), vectorX.end(), yMaxSupport.begin(),
                                            [&](double x) -> double {
                                                return pairABSupportMax.first * x + pairABSupportMax.second;
                                            });
                                ImPlot::PlotLine(plotNameMax.c_str(), &vectorX[0], &yMaxSupport[0],
                                                vSize);

                                std::vector<double> yMinSupport;
                                yMinSupport.resize(vSize);
                                auto &pairABSupportMin = data.supportMinByTokenName[tokenName].back();
                                std::string plotNameMin{
                                        "MIN(Y = " + std::to_string(pairABSupportMin.first) + " X + " +
                                        std::to_string(pairABSupportMin.second) +
                                        ")"};
                                std::transform(vectorX.begin(), vectorX.end(), yMinSupport.begin(),
                                            [&](double x) -> double {
                                                return pairABSupportMin.first * x + pairABSupportMin.second;
                                            });
                                ImPlot::PlotLine(plotNameMin.c_str(), &vectorX[0], &yMinSupport[0],
                                                vSize);
                            }
                        }

                        auto& vectorMovingAverage = data.movingAverageByTokenName[tokenName];
                        std::vector<double> vectorMovingX, vectorMovingY;

                        Utils::splitDataInTwoVectors(vectorMovingAverage,vectorMovingX,vectorMovingY,0,vectorMovingAverage.size());
                        ImPlot::PlotLine("Average",&vectorMovingX[0+offset],&vectorMovingY[0+offset],vectorMovingAverage.size()-offset);

                        displayHiLows(data,tokenName,offset);
                        displayTrendingChanges(data,tokenName,offset);
                        
                        ImPlot::EndPlot();
                    }
                }
            }
        }
    });
    ImGui::End();
}
void DataGraphicDisplayer::displayHiLows(Data& data,const std::string& symbol,unsigned int offset) const
{
    auto& his = data.hisByTokenName[symbol];

    std::vector<double> vectorX, vectorY;

    Utils::splitDataInTwoVectors(his,vectorX,vectorY,0,his.size());

    vectorX.erase(std::remove_if(vectorX.begin(),vectorX.end(),[offset](auto& element){return element<offset;}),vectorX.end());

    int i = vectorY.size()-vectorX.size();

    vectorY.erase(std::remove_if(vectorY.begin(),vectorY.end(),[&i](auto& element){return i-->0;}),vectorY.end());
    ImPlot::PlotScatter("His", &vectorX[0], &vectorY[0], vectorX.size());
    

    auto& lows = data.lowsByTokenName[symbol];

    std::vector<double> vectorXLows, vectorYLows;

    Utils::splitDataInTwoVectors(lows,vectorXLows,vectorYLows,0,lows.size());

    vectorXLows.erase(std::remove_if(vectorXLows.begin(),vectorXLows.end(),[offset](auto& element){return element<offset;}),vectorXLows.end());

    int j = vectorYLows.size()-vectorXLows.size();

    vectorYLows.erase(std::remove_if(vectorYLows.begin(),vectorYLows.end(),[&j](auto& element){return j-->0;}),vectorYLows.end());
    ImPlot::PlotScatter("Lows", &vectorXLows[0], &vectorYLows[0], vectorXLows.size());

}
void DataGraphicDisplayer::displayTrendingChanges(Data& data, const std::string& symbol, unsigned int offset) const
{
    std::vector<double> vectorX,vectorY;
    auto& marks = data.trendingChangeByTokenName[symbol];

    for(auto& mark : marks)
    {
        if(mark.first>=offset)
        {
            vectorX.push_back(mark.first);
            /*vectorY.push_back(mark.second);

            vectorX.push_back(mark.first);
            vectorY.push_back(mark.second);*/
        }
    }
    ImPlot::PlotVLines("Trending", &vectorX[0],vectorX.size());
    
    

}
void DataGraphicDisplayer::displayLinearRegression(std::vector<double>& vectorX, std::pair<double,double>& pairAB)
{
    std::vector<double> yLinear;
    yLinear.resize(vectorX.size());

    std::transform(vectorX.begin(), vectorX.end(), yLinear.begin(), [&](double x) -> double {
        return pairAB.first * x + pairAB.second;
    });

    // TODO : prolongate

    std::string plotName{
            "R(Y = " + std::to_string(pairAB.first) + " X + " + std::to_string(pairAB.second) +
            ")"};
    ImPlot::PushStyleColor(ImPlotColormap_Plasma, 1);
    ImPlot::PlotLine(plotName.c_str(), &vectorX[0], &yLinear[0], vectorX.size());
    ImPlot::PopStyleColor();

}
void DataGraphicDisplayer::displaySearch(entityx::EntityManager& entityManager, entityx::EventManager& events)
{
    ImGui::Begin("Search");
    ImGui::Text("Search Regexp :");

    if (ImGui::InputText("", &mRegexpFilter/*,mFilterByKeyWord?ImGuiInputTextFlags_ReadOnly:ImGuiInputTextFlags_None*/))
    {
        events.emit<FilterByRegexpEnteredEvent>(mRegexpFilter);
    }

    ImGui::End();
}
