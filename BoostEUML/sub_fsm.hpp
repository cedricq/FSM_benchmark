#ifndef SUB_FSM_HPP
#define SUB_FSM_HPP

#include <iostream>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "events.hpp"

using namespace std;
using namespace boost::msm::front::euml;
namespace msm = boost::msm;

namespace
{
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

}  // namespace
#endif
