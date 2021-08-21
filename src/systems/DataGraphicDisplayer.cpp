//
// Created by dwarf on 18/07/2021.
//
#include "../../include/systems/DataGraphicDisplayer.hpp"
#include "../../include/Utils.hpp"
bool DataGraphicDisplayer::mFilterByKeyWord = false;
bool DataGraphicDisplayer::mShowOnlyAccPos = false;
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
    displayOptions(es);
}
void DataGraphicDisplayer::displayOptions(entityx::EntityManager &es) const
{
    ImGui::Begin("Options");
    ImGui::Checkbox("Display only token with +Acc", &mShowOnlyAccPos);
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
                    auto &pairAB = data.linearRegressionByTokenName[tokenWithHistory.first].back();

                    double currentPrice = vectorPrice.back().second;
                    double oldPrice = (*(vectorPrice.end() - 2)).second;
                    double acc = pairAB.first;

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

                if ((mRegexpFilter.empty()||std::regex_match(tokenWithoutAgainst, std::regex(mRegexpFilter)))&&(!mFilterByKeyWord ||
                    mFilterByKeyWord && Utils::setContains(mTrendingTokens, tokenWithoutAgainst)))
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

            if((mRegexpFilter.empty()||std::regex_match(tokenWithoutAgainst, std::regex(mRegexpFilter)))&&
            !mFilterByKeyWord||mFilterByKeyWord&&Utils::setContains(mTrendingTokens,tokenWithoutAgainst)) {

                auto vectorPrice = tokenWithHistory.second;
                std::vector<double> vectorX;
                vectorX.resize(vectorPrice.size());
                std::transform(vectorPrice.begin(),vectorPrice.end(),vectorX.begin(),[](std::pair<double,double> p)->double{return p.first;});

                std::vector<double> vectorY;
                vectorY.resize(vectorPrice.size());
                std::transform(vectorPrice.begin(),vectorPrice.end(),vectorY.begin(),[](std::pair<double,double> p)->double{return p.second;});

                auto tokenName = tokenWithHistory.first.c_str();
                auto& pairAB = data.linearRegressionByTokenName[tokenName].back();
                if(!mShowOnlyAccPos||mShowOnlyAccPos&&pairAB.first > 0) {
                    const unsigned int maxSize = std::stoi(Utils::config["maxdisplayedvalues"]);
                    int offset = vectorX.size() - maxSize;
                    if(offset<0)
                    {
                        offset=0;
                    }

                    double yMaxValue = *std::max_element(vectorY.begin(),vectorY.end());
                    ImPlot::SetNextPlotLimits(offset, vectorX.size()+2, yMaxValue - (yMaxValue*0.2f), yMaxValue + (yMaxValue*0.2f));

                    if (ImPlot::BeginPlot(tokenName)) {

                        ImPlot::PlotScatter("Price", &vectorX[0+offset], &vectorY[0+offset], vectorPrice.size()-offset);

                        std::vector<double> yLinear;
                        yLinear.resize(vectorX.size());

                        std::transform(vectorX.begin(), vectorX.end(), yLinear.begin(), [&](double x) -> double {
                            return pairAB.first * x + pairAB.second;
                        });
                        std::string plotName{
                                "R(Y = " + std::to_string(pairAB.first) + " X + " + std::to_string(pairAB.second) +
                                ")"};
                        ImPlot::PushStyleColor(ImPlotColormap_Plasma, 1);
                        ImPlot::PlotLine(plotName.c_str(), &vectorX[0+offset], &yLinear[0+offset], vectorPrice.size()-offset);
                        ImPlot::PopStyleColor();
                        std::vector<double> yMaxSupport;
                        yMaxSupport.resize(vectorX.size());
                        auto& pairABSupportMax = data.supportMaxByTokenName[tokenName].back();
                        std::string plotNameMax{
                                "MAX(Y = " + std::to_string(pairABSupportMax.first) + " X + " + std::to_string(pairABSupportMax.second) +
                                ")"};
                        std::transform(vectorX.begin(), vectorX.end(), yMaxSupport.begin(), [&](double x) -> double {
                            return pairABSupportMax.first * x + pairABSupportMax.second;
                        });
                        ImPlot::PlotLine(plotNameMax.c_str(), &vectorX[0+offset], &yMaxSupport[0+offset], vectorPrice.size()-offset);

                        std::vector<double> yMinSupport;
                        yMinSupport.resize(vectorX.size());
                        auto& pairABSupportMin = data.supportMinByTokenName[tokenName].back();
                        std::string plotNameMin{
                                "MIN(Y = " + std::to_string(pairABSupportMin.first) + " X + " + std::to_string(pairABSupportMin.second) +
                                ")"};
                        std::transform(vectorX.begin(), vectorX.end(), yMinSupport.begin(), [&](double x) -> double {
                            return pairABSupportMin.first * x + pairABSupportMin.second;
                        });
                        ImPlot::PlotLine(plotNameMin.c_str(), &vectorX[0+offset], &yMinSupport[0+offset], vectorPrice.size()-offset);

                        ImPlot::EndPlot();
                    }
                }
            }
        }
    });
    ImGui::End();
}

void DataGraphicDisplayer::displaySearch(entityx::EntityManager& entityManager, entityx::EventManager& events)
{
    ImGui::Begin("Search");
    ImGui::Text("Search Regexp :");

    if (ImGui::InputText("", &mRegexpFilter,mFilterByKeyWord?ImGuiInputTextFlags_ReadOnly:ImGuiInputTextFlags_None))
    {
        events.emit<FilterByRegexpEnteredEvent>(mRegexpFilter);
    }

    ImGui::End();
}
