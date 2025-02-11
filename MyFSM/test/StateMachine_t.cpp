#include <gtest/gtest.h>

#include "StateMachine.h"

#include <iostream>

using namespace FSM;

MAKE_INTERNAL_EVENT(NoEvent);
MAKE_INTERNAL_EVENT(StartEvent);
MAKE_INTERNAL_EVENT(FinishEvent);
MAKE_INTERNAL_EVENT(SubEvent);
MAKE_INTERNAL_EVENT(GuardEvent);
MAKE_INTERNAL_EVENT(ActionEvent);
MAKE_INTERNAL_EVENT(End);

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


void tick(StateManager& sm, EventId const& event)
{
    sm->run();
    sm->handleEvent(event) ;
}

bool flag_guard = false;
bool flag_delegated = false;

TEST(StateMachine, all)
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
    ADD_TRANSITION_GUARD (stateA,
                            GuardEvent,
                            [] {return flag_guard;},
                            stateB);

    ADD_TRANSITION_GUARD_ACTION (stateA,
                                 ActionEvent,
                                 [] {return true;},
                                 [] {flag_delegated = true; std::cout <<"!! action delegated !!" <<std::endl;},
                                 stateB);

    ADD_TRANSITION (stateB,    FinishEvent, stateA);


    // Substate transitions
    ADD_TRANSITION (subState,  FinishEvent, stateA);
    ADD_TRANSITION (stateB,    End,         Exit);

    int onEntry = 0;
    int onRun   = 0;
    int onExit  = 0;
    SET_ON_ENTRY(stateA, [&]{ ++onEntry; });
    SET_RUN(stateA, [&]{ ++onRun; });
    SET_ON_EXIT(stateA, [&]{ ++onExit; });

    ASSERT_TRUE(mainState->isStateManager());
    ASSERT_FALSE(stateA->isStateManager());
    ASSERT_EQ(stateA, mainState->getInitState());
    ASSERT_EQ(stateA, stateA->getInitState());

    mainState->start();

    // Nothing happen on NoEvent
    ASSERT_TRUE(mainState->isState("A"));
    tick(mainState, NoEvent);
    ASSERT_TRUE(mainState->isState("A"));

    // Standard transitions
    tick(mainState, StartEvent);
    ASSERT_TRUE(mainState->isState("B"));
    {
        auto state = mainState->getState();
        EXPECT_EQ(state, stateB);
    }

    tick(mainState, FinishEvent);
    ASSERT_TRUE(mainState->isState("A"));

    // Entering sub state machine
    tick(mainState, SubEvent);
    ASSERT_TRUE(mainState->isState("C"));

    // Exiting sub state machine
    tick(mainState, FinishEvent);
    ASSERT_TRUE(mainState->isState("A"));

    // Guard is false, no transition
    flag_guard = false;
    tick(mainState, GuardEvent);
    ASSERT_TRUE(mainState->isState("A"));
    // Guard is true, transition
    flag_guard = true;
    tick(mainState, GuardEvent);
    ASSERT_TRUE(mainState->isState("B"));

    // Back to state A
    tick(mainState, FinishEvent);
    ASSERT_TRUE(mainState->isState("A"));

    // Action delegated
    ASSERT_EQ(false, flag_delegated);
    tick(mainState, ActionEvent);
    ASSERT_EQ(true, flag_delegated);
    ASSERT_TRUE(mainState->isState("B"));

    tick(mainState, End);
    ASSERT_TRUE(mainState->isState("B"));

    ASSERT_EQ(4, onEntry);
    ASSERT_EQ(6, onRun);
    ASSERT_EQ(4, onExit);
}
