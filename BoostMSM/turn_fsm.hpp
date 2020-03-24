#ifndef BOOSTMSM_TEST_TURN_FSM_HPP_
#define BOOSTMSM_TEST_TURN_FSM_HPP_

#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace
{

// the main state machine contains a state which is himself a state machine
// as you see, no need to declare it anywhere so it can be developed separately
// by another team in another module. For simplicity I just declare it inside main fsm
struct Turn_ : public msm::front::state_machine_def<Turn_>
{
    // when playing, the CD is loaded and we are in either pause or playing (duh)
    template <class Event,class FSM>
    void on_entry(Event const&,FSM& ) {std::cout << "entering: Turn" << std::endl;}
    template <class Event,class FSM>
    void on_exit(Event const&,FSM& ) {std::cout << "leaving: Turn" << std::endl;}

    // The list of FSM states
    struct Trigger : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "starting: First Step" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "finishing: First Step" << std::endl;}

    };

    struct FirstStep : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "starting: First Step" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "finishing: First Step" << std::endl;}

    };
    struct Turning : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "starting: Turning" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "finishing: Turning" << std::endl;}
    };
    struct LastStep : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "starting: Last Step" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "finishing: Last Step" << std::endl;}
    };
    // the initial state. Must be defined
    typedef Trigger initial_state;
    // transition actions
    void StartTurning(imu_detection const&)  { std::cout << "Turn - Action - imu detection - start turning\n"; }
    void StopTurning(standing_button const&) { std::cout << "Turn - Action - standing button - stop turning\n"; }
    // guard conditions

    typedef Turn_ trn; // makes transition table cleaner
    // Transition table for Walk
    struct transition_table : mpl::vector3<
        //      Start       Event                    Next             Action               Guard
        //    +------------+----------------------+---------------+-----------------------+---------+
        a_row < Trigger    , imu_detection        , FirstStep     , &trn::StartTurning >,
        _row  < FirstStep  , first_step_completed , Turning             >,
        a_row < Turning    , standing_button      , LastStep      , &trn::StopTurning  >
        //    +------------+----------------------+---------------+------------------------+--------+
    > {};
    // Replaces the default no-transition response.
    template <class FSM,class Event>
    void no_transition(Event const& e, FSM&,int state)
    {
        std::cout << "no transition from state " << state
            << " on event " << typeid(e).name() << std::endl;
    }
};

}
#endif /* BOOSTMSM_TEST_Turn_FSM_HPP_ */
