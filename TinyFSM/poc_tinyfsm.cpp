/*
 * poc_tinyfsm.cpp
 *
 *  Created on: 17 mars 2020
 *      Author: cedric
 */


#include "tinyfsm.hpp"
#include <iostream>

struct Installation;
struct Sitting;
struct Standing;
struct Walking;
struct Exercising;
struct Turning;



// ----------------------------------------------------------------------------
// 1. Event Declarations
//
struct Toggle : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// 2. State Machine Base Class Declaration
//
struct State : tinyfsm::Fsm<State>
{
  static void reset(void);

  virtual void react(Toggle const &) { };

  // alternative: enforce handling of Toggle in all states (pure virtual)
  //virtual void react(Toggle const &) = 0;

  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void) { };  /* no exit actions */

  // alternative: enforce entry actions in all states (pure virtual)
  //virtual void entry(void) = 0;
};

void State::reset() {
  tinyfsm::StateList<Installation, Sitting, Standing, Walking, Exercising, Turning>::reset();
}


// ----------------------------------------------------------------------------
// 3. State Declarations
//
struct Installation : State
{
  void entry() override { std::cout << "* State is Installation" << std::endl; };
  void react(Toggle const &) override { transit<Sitting>(); };
};

struct Sitting : State
{
  void entry() override { std::cout << "* State is Sitting" << std::endl; };
  void react(Toggle const &) override { transit<Standing>(); };
};

struct Standing : State
{
  void entry() override { std::cout << "* State is Standing" << std::endl; };
  void react(Toggle const &) override { transit<Walking>(); };
};

struct Walking : State
{
  void entry() override { std::cout << "* State is Walking" << std::endl; };
  void react(Toggle const &) override { transit<Standing>(); };
};

struct Exercising: State
{
  void entry() override { std::cout << "* State is Exercising" << std::endl; };
  void react(Toggle const &) override { transit<Standing>(); };
};

struct Turning : State
{
  void entry() override { std::cout << "* State is Turning" << std::endl; };
  void react(Toggle const &) override { transit<Standing>(); };
};

FSM_INITIAL_STATE(State, Installation)


// ----------------------------------------------------------------------------
// 4. State Machine List Declaration (dispatches events to multiple FSM's)
//
// In this example, we only have a single state machine, no need to use FsmList<>:
//using fsm_handle = tinyfsm::FsmList< State >;
using fsm_handle = State;


// ----------------------------------------------------------------------------
// Main
//
int main()
{
  // instantiate events
  Toggle toggle;

  fsm_handle::start();

  while(1)
  {
    char c;
    std::cout << std::endl << "t=Toggle, r=Restart, q=Quit ? ";
    std::cin >> c;
    switch(c) {
    case 't':
      std::cout << "> Toggling switch..." << std::endl;
      fsm_handle::dispatch(toggle);
      // alternative: instantiating causes no overhead (empty declaration)
      //fsm_handle::dispatch(Toggle());
      break;
    case 'r':
	  State::reset();
	  State::start();
	  break;
    case 'q':
      return 0;
    default:
      std::cout << "> Invalid input" << std::endl;
    };
  }
}


