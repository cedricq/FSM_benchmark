#ifndef WALK_FSM_HPP
#define WALK_FSM_HPP

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
struct Walk_ : public msm::front::state_machine_def<Walk_>
{
    // when playing, the CD is loaded and we are in either pause or playing (duh)
    template <class Event,class FSM>
    void on_entry(Event const&,FSM& ) {std::cout << "entering: Walk" << std::endl;}
    template <class Event,class FSM>
    void on_exit(Event const&,FSM& ) {std::cout << "leaving: Walk" << std::endl;}

    // The list of FSM states
    struct TriggerWalk : public msm::front::state<>
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
    struct Walking : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "starting: Walking" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "finishing: Walking" << std::endl;}
    };
    struct LastStep : public msm::front::state<>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "starting: Last Step" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "finishing: Last Step" << std::endl;}
    };
    // the initial state. Must be defined
    typedef TriggerWalk initial_state;
    // transition actions
    void StartWalking(imu_detection const&)  { std::cout << "Walk - Action - imu detection - start walking\n"; }
    void StopWalking(standing_button const&) { std::cout << "Walk - Action - standing button - stop walking\n"; }
    // guard conditions

    typedef Walk_ wlk; // makes transition table cleaner
    // Transition table for Walk
    struct transition_table : mpl::vector3<
        //      Start       Event                    Next             Action               Guard
        //    +------------+----------------------+---------------+-----------------------+---------+
        a_row < TriggerWalk, imu_detection        , FirstStep     , &wlk::StartWalking >,
        _row  < FirstStep  , first_step_completed , Walking             >,
        a_row < Walking    , standing_button      , LastStep      , &wlk::StopWalking  >
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

    // Pick a back-end
    typedef msm::back::state_machine<Walk_> wlk;

}
#endif /* BOOSTMSM_TEST_WALK_FSM_HPP_ */
