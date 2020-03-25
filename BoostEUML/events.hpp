#ifndef EVENTS_HPP_
#define EVENTS_HPP_

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/euml/euml.hpp>

namespace  // Concrete FSM implementation
{
    // events
    BOOST_MSM_EUML_EVENT(sitting_button)
    BOOST_MSM_EUML_EVENT(standing_up_button)
    BOOST_MSM_EUML_EVENT(walking_button)
    BOOST_MSM_EUML_EVENT(standing_button)
    BOOST_MSM_EUML_EVENT(exercising_button)
    BOOST_MSM_EUML_EVENT(turning_button)
    BOOST_MSM_EUML_EVENT(sitting_down_button)
    BOOST_MSM_EUML_EVENT(installation_button)

    BOOST_MSM_EUML_EVENT(kill)
    BOOST_MSM_EUML_EVENT(time_out)

    BOOST_MSM_EUML_EVENT(imu_detection)
    BOOST_MSM_EUML_EVENT(imu_timeout)
    BOOST_MSM_EUML_EVENT(first_step_completed)
    BOOST_MSM_EUML_EVENT(finished)

    BOOST_MSM_EUML_EVENT(pause)
    BOOST_MSM_EUML_EVENT(gentle_trap)
    BOOST_MSM_EUML_EVENT(trap)
    BOOST_MSM_EUML_EVENT(alert)
}

#endif /* BOOSTMSM_EVENTS_HPP_ */
