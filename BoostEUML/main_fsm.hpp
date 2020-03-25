#ifndef MAIN_FSM_HPP
#define MAIN_FSM_HPP

#include <iostream>
// back-end
//#include <boost/msm/front/euml/stl.hpp>
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"
//#include "walk_fsm.hpp"
//#include "turn_fsm.hpp"

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

    //BOOST_MSM_EUML_STATE(( installation_entry, installation_exit),Installation)
    BOOST_MSM_EUML_STATE((installation_entry, installation_exit, attributes_ << stopsEnabled),Installation)
    BOOST_MSM_EUML_STATE((),Sitting)
    BOOST_MSM_EUML_STATE((),StandingUp)
    BOOST_MSM_EUML_STATE((),Standing)
    BOOST_MSM_EUML_STATE((),SittingDown)

    // Substates
    BOOST_MSM_EUML_STATE((),TriggerWalk)
    BOOST_MSM_EUML_STATE((),FirstStep)
    BOOST_MSM_EUML_STATE((),Walking)
    BOOST_MSM_EUML_STATE((),Turning)
    BOOST_MSM_EUML_STATE((),LastStep)

   BOOST_MSM_EUML_TRANSITION_TABLE((
        //  +------------------------------------------------------------------------------+
            FirstStep   == TriggerWalk       + imu_detection ,
            Walking     == FirstStep         + first_step_completed ,
            LastStep    == Walking           + standing_button
        //  +------------------------------------------------------------------------------+
        ), walking_transition_table )

    BOOST_MSM_EUML_DECLARE_STATE_MACHINE( (walking_transition_table, //STT
                                        init_ << TriggerWalk // Init State
                                        ),Walk_)

    // choice of back-end
    typedef msm::back::state_machine<Walk_> Walk_type;
    Walk_type const Walk;

    BOOST_MSM_EUML_TRANSITION_TABLE((
         //  +------------------------------------------------------------------------------+
             FirstStep   == TriggerWalk       + imu_detection ,
             Walking     == FirstStep         + first_step_completed ,
             LastStep    == Turning           + standing_button
         //  +------------------------------------------------------------------------------+
         ), turning_transition_table )

    BOOST_MSM_EUML_DECLARE_STATE_MACHINE( (walking_transition_table, //STT
                                         init_ << TriggerWalk // Init State
                                         ),Turn_)

    // choice of back-end
    typedef msm::back::state_machine<Turn_> Turn_type;
    Turn_type const Turn;

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

}  // namespace
#endif
