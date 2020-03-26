#ifndef MAIN_FSM_HPP
#define MAIN_FSM_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"
#include "walk_fsm.hpp"
#include "turn_fsm.hpp"

namespace msm = boost::msm;
namespace mpl = boost::mpl;

namespace
{
    struct main_fsm_ : public msm::front::state_machine_def<main_fsm_>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Main FSM" << std::endl; }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Main FSM" << std::endl;}

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
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: StandingUp" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: StandingUp" << std::endl;}
        };

        struct Standing : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Standing" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Standing" << std::endl;}
        };

        struct SittingDown : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: SittingDown" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: SittingDown" << std::endl;}
        };

        typedef Installation initial_state;

        // transition actions
        void EnableStops(sitting_button const&)       { stopsEnabled = true; }
        void DisableStops(installation_button const&)    { stopsEnabled = false; }

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
            _row < Standing       , walking_button        , WalkFSM           >,
            _row < Standing       , turning_button        , TurnFSM           >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < SittingDown    , finished              , Sitting           >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < WalkFSM        , finished              , Standing          >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < TurnFSM        , finished              , Standing          >
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
    typedef msm::back::state_machine<main_fsm_> MainFSM;


    struct overall_fsm_ : public msm::front::state_machine_def<overall_fsm_>
    {
        template <class Event,class FSM>
        void on_entry(Event const& ,FSM&) {std::cout << "entering: Overall FSM" << std::endl; }
        template <class Event,class FSM>
        void on_exit(Event const&,FSM& ) {std::cout << "leaving: Overall FSM" << std::endl;}

        struct GentleTrap : public msm::front::state<>
        {
            // every (optional) entry/exit methods get the event passed.
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: GentleTrap" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: GentleTrap" << std::endl;}
        };
        struct Trap : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Trap" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Trap" << std::endl;}
        };

        struct Alert : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Alert" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Alert" << std::endl;}
        };

        struct Initializing : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Initializing" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Initializing" << std::endl;}
        };

        struct Running : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Running" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Running" << std::endl;}
        };

        struct Closing : public msm::front::state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Closing" << std::endl;}
            template <class Event,class FSM>
            void on_exit(Event const&,FSM& ) {std::cout << "leaving: Closing" << std::endl;}
        };

        struct Closed : public msm::front::terminate_state<>
        {
            template <class Event,class FSM>
            void on_entry(Event const&,FSM& ) {std::cout << "entering: Closed" << std::endl;}
        };

        typedef mpl::vector2<MainFSM,Initializing> initial_state;

        // transition actions
        void EnableStops(sitting_button const&)       { stopsEnabled = true; }
        void DisableStops(installation_button const&)    { stopsEnabled = false; }

        typedef overall_fsm_ m; // makes transition table cleaner

        struct transition_table : mpl::vector<
            //      Start            Event                   Next                  Action               Guard
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < MainFSM        , gentle_trap           , GentleTrap          >,
            _row < MainFSM        , trap                  , Trap                >,
            _row < MainFSM        , alert                 , Alert               >,
            _row < MainFSM        , kill                  , Alert               >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < GentleTrap     , finished              , Trap                >,
            _row < GentleTrap     , gentle_trap           , Trap                >,
            _row < GentleTrap     , trap                  , Trap                >,
            _row < GentleTrap     , time_out              , Trap                >,
            _row < GentleTrap     , alert                 , Alert               >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Trap           , alert                 , Alert               >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Initializing   , finished              , Running             >,
            _row < Running        , kill                  , Closing             >,
            _row < Closing        , finished              , Closed              >
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
    typedef msm::back::state_machine<overall_fsm_> OverallFSM;
}
#endif
