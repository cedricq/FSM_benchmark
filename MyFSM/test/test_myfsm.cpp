#include "gtest/gtest.h"

#include "../myfsm.hpp"

#include <iostream>

using namespace core;

EventId NoEvent{0, "NoEvent"};
EventId StartEvent{1, "StartEvent"};
EventId FinishEvent{2, "FinishEvent"};
EventId SubEvent{3, "SubEvent"};
EventId GuardEvent{4, "GuardEvent"};
EventId ActionEvent{5, "ActionEvent"};

class StateA : public State_ 
{
 public:
    StateA() : State_("A") {};
    virtual ~StateA() {};
};

class StateB : public State_ 
{
 public:
    StateB() : State_("B") {};
    virtual ~StateB() {};
};

class StateC : public State_ 
{
 public:
    StateC() : State_("C") {};
    virtual ~StateC() {};
};


void Tick(StateManager& sm, EventId& event)
{    
    sm->run();
    sm->handleEvent(event) ;
}

TEST(MyFSM, test)
{
    // Listing states
    State stateA = std::make_shared<StateA>();
    State stateB = std::make_shared<StateB>();
    StateManager mainState = std::make_shared<StateManagerClass_>("Main");
    SET_INIT_STATE(mainState, stateA);
    
    State stateC = std::make_shared<StateC>();
    StateManager subState = std::make_shared<StateManagerClass_>("Sub");
    SET_INIT_STATE(subState, stateC);
    
    // MainState transitions
    ADD_TRANSITION (stateA,    StartEvent,  stateB);
    ADD_TRANSITION (stateA,    SubEvent,    subState);
    ADD_TRANSITION_GUARD (stateA,  GuardEvent, [] {return false;}, stateB);

    ADD_TRANSITION_GUARD_ACTION (stateA,  
                                 ActionEvent, 
                                 [] {return true;}, [] {std::cout <<"!! action delegated !!" <<std::endl;}, 
                                 stateB);
    
    ADD_TRANSITION (stateB,    FinishEvent, stateA);
    
    
    // Substate transitions
    ADD_TRANSITION (subState,  FinishEvent, stateA);
    
    mainState->start();
    
    ASSERT_TRUE(mainState->isState("A"));
     
    Tick(mainState, NoEvent);

    ASSERT_TRUE(mainState->isState("A"));
     
    Tick(mainState, SubEvent);
    ASSERT_TRUE(mainState->isState("C"));
    ASSERT_TRUE(subState->isState("C"));
     
    Tick(mainState, FinishEvent);
    ASSERT_TRUE(mainState->isState("A"));
    
    Tick(mainState, StartEvent);
    ASSERT_TRUE(mainState->isState("B"));
    
    Tick(mainState, FinishEvent);
    ASSERT_TRUE(mainState->isState("A"));
    
    Tick(mainState, GuardEvent);
    ASSERT_TRUE(mainState->isState("A"));
    
    Tick(mainState, ActionEvent);
    ASSERT_TRUE(mainState->isState("B"));
}


