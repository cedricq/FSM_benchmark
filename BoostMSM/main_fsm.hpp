#ifndef POC_BOOSTMSM_HPP
#define POC_BOOSTMSM_HPP

#include <iostream>
// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"
#include "walk_fsm.hpp"
#include "turn_fsm.hpp"

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace  // Concrete FSM implementation
{
    // A "complicated" event type that carries some data.
    struct recover
    {
        recover(bool isVertical)
            : isVertical_(isVertical)
        {}

        bool isVertical_;
    };

    // front-end: define the FSM structure
    struct main_fsm_ : public msm::front::state_machine_def<main_fsm_>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Main FSM" << std::endl; }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Main FSM" << std::endl;}

        // The list of FSM states
        struct Installation : public msm::front::state<>
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Installation" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Installation" << std::endl;}
        };
        struct Sitting : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Sitting" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Sitting" << std::endl;}
        };

        struct StandingUp : public msm::front::state<>
        {
            // when stopped, the CD is loaded
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: StandingUp" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: StandingUp" << std::endl;}
        };

        struct Standing : public msm::front::state<>
        {
            // when stopped, the CD is loaded
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Standing" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Standing" << std::endl;}
        };

        struct SittingDown : public msm::front::state<>
        {
            // when stopped, the CD is loaded
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: SittingDown" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: SittingDown" << std::endl;}
        };

        // back-end
        typedef msm::back::state_machine<Walk_> Walk;
        typedef msm::back::state_machine<Turn_> Turn;

        // the initial state of the player SM. Must be defined
        typedef Installation initial_state;

        // transition actions
        void EnableStops(sitting_button const&)       { stopsEnabled = true; }
        void DisableStops(installation_button const&)    { stopsEnabled = false; }

        // guard conditions

        typedef main_fsm_ m; // makes transition table cleaner

        struct transition_table : mpl::vector<
            //      Start            Event                   Next                  Action               Guard
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            a_row < Installation  , sitting_button        , Sitting           , &m::EnableStops         >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            a_row < Sitting       , installation_button   , Installation      , &m::DisableStops        >,
            _row <  Sitting       , standing_up_button    , StandingUp        >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < StandingUp     , finished              , Standing          >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Standing       , sitting_down_button   , SittingDown       >,
            _row < Standing       , walking_button        , Walk              >,
            _row < Standing       , turning_button        , Turn              >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < SittingDown    , finished              , Sitting           >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Walk           , finished              , Standing          >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Turn           , finished              , Standing          >
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
        > {};

        // Replaces the default no-transition response.
        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&,int state)
        {
            std::cout << "no transition from state " << state
                << " on event " << typeid(e).name() << std::endl;
        }

        bool stopsEnabled {false};

    };
    // Pick a back-end
    typedef msm::back::state_machine<main_fsm_> fsm;

}
#endif
