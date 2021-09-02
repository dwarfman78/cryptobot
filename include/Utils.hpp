//
// Created by dwarf on 17/07/2021.
//

#ifndef CRYPTOBOT_UTILS_HPP
#define CRYPTOBOT_UTILS_HPP
#include "components/Data.hpp"
struct TokenSearchComp
{
    bool operator()(const std::pair<unsigned int, std::string>& lhs, const std::pair<unsigned int, std::string>& rhs) const
    {
        return lhs.second < rhs.second;
    }
};
/**
 * Utilitary functions.
 */
class Utils
{
public:

    static void to_string(const Data& data){std::cout << &data << " binanceTokenPrice : " << data.binanceTokenPrice << std::endl;}
    /**
     * Tells if the set pSet contains the token pSearch.
     * @param pSet the set to be tested
     * @param pSearch the searched token name
     * @return true if the set contains the  token false otherwise.
     */
    static bool setContains(const std::set<std::pair<unsigned int,std::string>,TrendingTokenComp>& pSet, const std::string& pSearch)
    {
        auto result = std::find_if(pSet.begin(), pSet.end(), [pSearch](std::pair<unsigned int,std::string> p){return p.second == pSearch;});

        return result != pSet.end();
    }
    /**
     * Erases the pSubstring from the string pString.
     * @param pString original string by reference, this string will be modified.
     * @param pSubstring the substring to be removed.
     * @return true if pSubstring is found and removed from pString, false otherwise.
     */
    static bool eraseSubstring(std::string& pString, const std::string& pSubstring)
    {
        bool returnValue{true};
        std::size_t pos = pString.find(pSubstring);
        if(pos !=std::string::npos)
        {
            pString.erase(pos,pSubstring.length());
        }else
        {
            returnValue = false;
        }

        return returnValue;
    }
    /**
     * Tells if a token is to be selected for display / calculation.
     * @param filterRegexp Filter regexp.
     * @param filterByKeyword Filter by keyword.
     * @param token Current token.
     * @param keyWords keyWords list.
     * @return true is
     */
    static bool isTokenSelected(const std::string& filterRegexp, const bool filterByKeyword, const std::string& token, const std::set<std::pair<unsigned int, std::string>,TrendingTokenComp>& keyWords)
    {
        bool retour{filterRegexp.empty()&&!filterByKeyword};

        if (!filterRegexp.empty())
        {
            try
            {
                retour = std::regex_match(token, std::regex(filterRegexp));
            }
            catch(const std::regex_error& regexError)
            {
                std::cout << "Invalid regexp : " << regexError.what() << std::endl;
            }
        }
        if(filterByKeyword)
        {
            retour = retour || Utils::setContains(keyWords, token);
        }

        return retour;
    }
    static std::map<std::string,std::string> config;
};
#endif //CRYPTOBOT_UTILS_HPP
