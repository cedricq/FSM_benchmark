#ifndef MAIN_FSM_HPP
#define MAIN_FSM_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"
#include "sub_fsm.hpp"

using namespace std;
using namespace boost::msm::front::euml;
namespace msm = boost::msm;

namespace
{
    //states
    BOOST_MSM_EUML_DECLARE_ATTRIBUTE(bool, stopsEnabled)

    BOOST_MSM_EUML_ACTION(installation_entry)
    {
        template <class Evt,class Fsm,class State>
        void operator()(Evt const& ,Fsm& fsm,State& state)
        {
            fsm.get_attribute(stopsEnabled) =false;
            std::cout<<"install entry - Disable stops" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(installation_exit)
    {
        template <class Evt,class Fsm,class State>
        void operator()(Evt const& ,Fsm& fsm,State& state)
        {
            fsm.get_attribute(stopsEnabled) =true;
            std::cout<<"install exit - Enable stops" <<std::endl;
        }
    };

    BOOST_MSM_EUML_STATE((installation_entry, installation_exit, attributes_ << stopsEnabled),Installation)
    BOOST_MSM_EUML_STATE((),Sitting)
    BOOST_MSM_EUML_STATE((),StandingUp)
    BOOST_MSM_EUML_STATE((),Standing)
    BOOST_MSM_EUML_STATE((),SittingDown)

    // Actions
    BOOST_MSM_EUML_ACTION(EnableStops)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - Enable stops" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(DisableStops)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - Disable stops" <<std::endl;
        }
    };

    // Guards
    BOOST_MSM_EUML_ACTION(is_chair_height_valid)
    {
        template <class Fsm,class Evt,class SourceState,class TargetState>
        bool operator()(Evt const& ,Fsm& ,SourceState&,TargetState& )
        {
            std::cout<<"chair height is valid" <<std::endl;
            return true;
        }
    };

    BOOST_MSM_EUML_ACTION(is_vertical)
    {
        template <class Fsm,class Evt,class SourceState,class TargetState>
        bool operator()(Evt const& ,Fsm& ,SourceState&,TargetState& )
        {
            std::cout<<"exo is vertical" <<std::endl;
            return false;
        }
    };

    BOOST_MSM_EUML_TRANSITION_TABLE((
          Installation == Sitting         + installation_button  / DisableStops,
          //  +------------------------------------------------------------------------------+
          Sitting      == Installation    + sitting_button [not is_vertical]  / EnableStops,
          Sitting      == SittingDown     + finished ,
          //  +------------------------------------------------------------------------------+
          StandingUp   == Sitting         + standing_up_button [is_chair_height_valid],
          //  +------------------------------------------------------------------------------+
          Standing     == StandingUp      + finished ,
          Standing     == Walk            + finished ,
          Standing     == Turn            + finished ,
          //  +------------------------------------------------------------------------------+
          SittingDown  == Standing        + sitting_down_button ,
          //  +------------------------------------------------------------------------------+
          Walk         == Standing        + walking_button ,
          //  +------------------------------------------------------------------------------+
          Turn         == Standing        + turning_button
          //  +------------------------------------------------------------------------------+
         ), main_transition_table)

    BOOST_MSM_EUML_DECLARE_STATE_MACHINE(( main_transition_table, //STT
                                        init_ << Installation,
                                        no_action,
                                        no_action,
                                        attributes_ << stopsEnabled
                                        ),
                                       main_fsm_) //fsm name

    typedef msm::back::state_machine<main_fsm_> main_fsm;
    main_fsm const MainFSM;


    BOOST_MSM_EUML_ACTION(initialisation_entry)
    {
        template <class Evt,class Fsm,class State>
        void operator()(Evt const& ,Fsm& ,State& )
        {
            std::cout<<"Initialisation entry" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(initialisation_exit)
    {
        template <class Evt,class Fsm,class State>
        void operator()(Evt const& ,Fsm& ,State& )
        {
            std::cout<<"Initialisation exit" <<std::endl;
        }
    };

    BOOST_MSM_EUML_STATE((),GentleTrap)
    BOOST_MSM_EUML_STATE((),Trap)
    BOOST_MSM_EUML_STATE((),Alert)

    BOOST_MSM_EUML_STATE((initialisation_entry, initialisation_exit),Initializing)
    BOOST_MSM_EUML_STATE((),Running)
    BOOST_MSM_EUML_STATE((),Closing)
    BOOST_MSM_EUML_STATE((),Closed)

    // Actions

    BOOST_MSM_EUML_ACTION(ToGentleTrap)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - To Gentle Trap" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(ToTrap)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - To Trap" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(ToAlert)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - To Alert" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(ToRunning)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - To Running" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(ToClosing)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - To Closing" <<std::endl;
        }
    };

    BOOST_MSM_EUML_ACTION(ToClosed)
    {
        template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
        {
            std::cout<<"ACTION - To Closed" <<std::endl;
        }
    };

    BOOST_MSM_EUML_TRANSITION_TABLE((
          GentleTrap   == MainFSM         + gentle_trap / ToGentleTrap,
          //  +------------------------------------------------------------------------------+
          Trap         == MainFSM         + trap        / ToTrap,
          Trap         == GentleTrap      + finished    / ToTrap,
          Trap         == GentleTrap      + gentle_trap / ToTrap,
          Trap         == GentleTrap      + trap        / ToTrap,
          Trap         == GentleTrap      + time_out    / ToTrap,
          //  +------------------------------------------------------------------------------+
          Alert        == MainFSM         + alert       / ToAlert,
          Alert        == MainFSM         + kill        / ToAlert,
          Alert        == GentleTrap      + alert       / ToAlert,
          Alert        == Trap            + alert       / ToAlert,
          //  +------------------------------------------------------------------------------+
          //  +------------------------------------------------------------------------------+
          Running      == Initializing    + finished    / ToRunning,
          Closing      == Running         + kill        / ToClosing,
          Closed       == Closing         + finished    / ToClosed
          //  +------------------------------------------------------------------------------+
         ), overall_transition_table)

    BOOST_MSM_EUML_DECLARE_STATE_MACHINE(( overall_transition_table,
                                        init_ << MainFSM << Initializing
                                        ),
                                       overall_fsm_)

    typedef msm::back::state_machine<overall_fsm_> overall_fsm;
}  // namespace
#endif
