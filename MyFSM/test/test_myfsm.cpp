#include "gtest/gtest.h"

#include "../myfsm.hpp"

#include <iostream>

using namespace core;

void Tick(StateManager& sm, EventId& event)
{    
    std::cout<<"Posting Event : " <<event.name.c_str() <<std::endl;
    sm->run();
    sm->handleEvent(event) ;
}

TEST(MyFSM, test)
{
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

EventId NoEvent         {0, "NoEvent"};
EventId AlertEvent      {1, "AlertEvent"};
EventId CriticalEvent   {3, "CriticalEvent"};
EventId EmergencyEvent  {4, "EmergencyEvent"};
EventId FinishEvent     {5, "FinishEvent"};
EventId ExerciseEvent   {6, "ExerciseEvent"};
EventId TurnEvent       {7, "TurnEvent"};
EventId WalkEvent       {8, "WalkEvent"};
EventId RepositionEvent {9, "RepositionEvent"};

StateManager supraState = std::make_shared<StateManagerClass_>("Supra");
StateManager mainState  = std::make_shared<StateManagerClass_>("Main");

void createSupraStateMachine()
{
    State Startup           = std::make_shared<State_>("Startup");
    State GentleTrap        = std::make_shared<State_>("GentleTrap");
    State Trap              = std::make_shared<State_>("Trap");
    State Emergency         = std::make_shared<State_>("Emergency");
    
    
    SET_INIT_STATE(supraState, Startup);
    
    ADD_TRANSITION (Startup,        FinishEvent,        mainState);
    
    ADD_TRANSITION (mainState,      AlertEvent,         GentleTrap);
    ADD_TRANSITION (mainState,      CriticalEvent,      Trap);
    ADD_TRANSITION (mainState,      EmergencyEvent,     Emergency);
    
    ADD_TRANSITION (GentleTrap,     AlertEvent,         Trap);
    ADD_TRANSITION (GentleTrap,     CriticalEvent,      Trap);
    ADD_TRANSITION (GentleTrap,     EmergencyEvent,     Emergency);
    
    ADD_TRANSITION (Trap,           CriticalEvent,      Emergency);
    ADD_TRANSITION (Trap,           EmergencyEvent,     Emergency);
}


void createMainStateMachine()
{
    State Sitting           = std::make_shared<State_>("Sitting");
    State Standing          = std::make_shared<State_>("Standing");
    State Exercising        = std::make_shared<State_>("Exercising");
    State Turning           = std::make_shared<State_>("Turning");
    State Walking           = std::make_shared<State_>("Walking");
    State Repositioning     = std::make_shared<State_>("Repositioning");
    
    SET_INIT_STATE(mainState, Sitting);
    
    ADD_TRANSITION (Sitting,        FinishEvent,        Standing);
    
    ADD_TRANSITION (Standing,       ExerciseEvent,      Exercising);
    ADD_TRANSITION (Standing,       TurnEvent,          Turning);
    ADD_TRANSITION (Standing,       WalkEvent,          Walking);
    ADD_TRANSITION (Standing,       RepositionEvent,    Repositioning);
    
    ADD_TRANSITION (Exercising,     FinishEvent,        Standing);
    ADD_TRANSITION (Turning,        FinishEvent,        Standing);
    ADD_TRANSITION (Walking,        FinishEvent,        Standing);
    ADD_TRANSITION (Repositioning,  FinishEvent,        Standing);

}

TEST(MyFSM, CompleteFSM)
{
    createSupraStateMachine();
    createMainStateMachine();
    
    ASSERT_TRUE(supraState->isState("Startup"));
    Tick(supraState, FinishEvent);
    ASSERT_TRUE(supraState->isState("Sitting"));
    Tick(supraState, FinishEvent);
    ASSERT_TRUE(supraState->isState("Standing"));
    Tick(supraState, ExerciseEvent);
    ASSERT_TRUE(supraState->isState("Exercising"));
    Tick(supraState, FinishEvent);
    ASSERT_TRUE(supraState->isState("Standing"));
    Tick(supraState, WalkEvent);
    ASSERT_TRUE(supraState->isState("Walking"));
    Tick(supraState, FinishEvent);
    ASSERT_TRUE(supraState->isState("Standing"));
    Tick(supraState, TurnEvent);
    ASSERT_TRUE(supraState->isState("Turning"));
    Tick(supraState, FinishEvent);
    ASSERT_TRUE(supraState->isState("Standing"));
    Tick(supraState, RepositionEvent);
    ASSERT_TRUE(supraState->isState("Repositioning"));
    Tick(supraState, AlertEvent);
    ASSERT_TRUE(supraState->isState("GentleTrap"));
    Tick(supraState, AlertEvent);
    ASSERT_TRUE(supraState->isState("Trap"));
    Tick(supraState, CriticalEvent);
    ASSERT_TRUE(supraState->isState("Emergency"));

}
