#include "gtest/gtest.h"

#include "../tinyfsm.hpp"

#include <iostream>


class Truc
{
public:    
    void action1 ()
    {
        std::cout <<"action1 !!" <<std::endl; 
    }
    
    bool IsOK() { std::cout <<"condition - IsOk : " <<flag <<std::endl;  return flag; }
        
    
    bool flag {false};
    int cnt{0};
};

// ----------------------------------------------------------------------------
// 1. Event Declarations
//

enum EventId
{
    NoEvent,
    StartFsm,
    StopFsm
};

struct EventSignal : tinyfsm::Event 
{
    EventId eventId {NoEvent};
};

// ----------------------------------------------------------------------------
// 1. State Declarations
//

struct StateA;
struct StateB;

// ----------------------------------------------------------------------------
// 2. State Machine Base Class Declaration
//
struct State : tinyfsm::Fsm<State>
{
  static void reset(void);

  virtual void react(EventSignal const &) { };

  // alternative: enforce handling of Toggle in all states (pure virtual)
  //virtual void react(Toggle const &) = 0;

  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void) { };  /* no exit actions */

  // alternative: enforce entry actions in all states (pure virtual)
  //virtual void entry(void) = 0;
  
  static void SetTruc(Truc* trucIn) {truc = trucIn;}
  static Truc* truc;
};

Truc* State::truc {0} ;

void State::reset() {
  tinyfsm::StateList<StateA, StateB>::reset();
}

// ----------------------------------------------------------------------------
// 3. State Declarations
//
struct StateA : State
{
  void entry() override { std::cout << "* State is StateA" << std::endl; };
  void react(EventSignal const & evt) override 
  { 
    if (evt.eventId == StartFsm && State::truc->IsOK() )
    {
      transit<StateB>(); 
    }
      
  };
};

struct StateB : State
{
  void entry() override { std::cout << "* State is StateB" << std::endl; };
  void react(EventSignal const &) override { transit<StateA>(); };
};

FSM_INITIAL_STATE(State, StateA)

// ----------------------------------------------------------------------------
// 4. State Machine List Declaration (dispatches events to multiple FSM's)
//
// In this example, we only have a single state machine, no need to use FsmList<>:
//using fsm_handle = tinyfsm::FsmList< State >;
using fsm_handle = State;

TEST(TinyFSM, test)
{
    // instantiate events
  EventSignal signal;
  
  Truc truc;

  fsm_handle::SetTruc(&truc);
  
  fsm_handle::start();
  
  fsm_handle::dispatch(signal);
  ASSERT_TRUE(fsm_handle::is_in_state<StateA>());
  
  signal.eventId = StartFsm;
  fsm_handle::dispatch(signal);
  ASSERT_TRUE(fsm_handle::is_in_state<StateA>());
  
  truc.flag = true;
  fsm_handle::dispatch(signal);
  ASSERT_TRUE(fsm_handle::is_in_state<StateB>());

  fsm_handle::dispatch(signal);
  ASSERT_TRUE(fsm_handle::is_in_state<StateA>());  
  
  fsm_handle::dispatch(signal);
  ASSERT_TRUE(fsm_handle::is_in_state<StateB>());
  
  State::reset();
  State::start();
  ASSERT_TRUE(fsm_handle::is_in_state<StateA>());
    
}




