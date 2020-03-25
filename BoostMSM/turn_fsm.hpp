#ifndef TURN_FSM_HPP
#define TURN_FSM_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace
{
    struct Turn_ : public msm::front::state_machine_def<Turn_>
    {
        template <class Event,class FSM>
        void on_entry(Event const&,FSM& ) {std::cout << "entering: Turn" << std::endl;}
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Turn" << std::endl;}

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

        typedef Trigger initial_state;

        // transition actions
        void StartTurning(imu_detection const&)  { std::cout << "Turn - Action - imu detection - start turning\n"; }
        void StopTurning(standing_button const&) { std::cout << "Turn - Action - standing button - stop turning\n"; }

        typedef Turn_ trn; // makes transition table cleaner

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

    // Pick a back-end
    typedef msm::back::state_machine<Turn_> TurnFSM;
}
#endif /* BOOSTMSM_TEST_Turn_FSM_HPP_ */
