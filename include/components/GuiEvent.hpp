//
// Created by dwarf on 21/08/2021.
//

#ifndef CRYPTOBOT_GUIEVENT_HPP
#define CRYPTOBOT_GUIEVENT_HPP

enum GuiEventType
{
    FILTER_BY_KEYWORD,
    FILTER_BY_REGEXP
};
template<GuiEventType EventType,class EventValueType>
struct GuiEvent
{
    GuiEvent(EventValueType pValue):value(pValue),type(EventType){}
    GuiEventType type;
    EventValueType value;
};
using FilterByKeywordClickedEvent = GuiEvent<GuiEventType::FILTER_BY_KEYWORD,bool>;
using FilterByRegexpEnteredEvent = GuiEvent<GuiEventType::FILTER_BY_REGEXP,std::string>;

#endif //CRYPTOBOT_GUIEVENT_HPP
