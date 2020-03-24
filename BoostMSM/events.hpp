/*
 * events.hpp
 *
 *  Created on: 24 mars 2020
 *      Author: cedric
 */

#ifndef BOOSTMSM_EVENTS_HPP_
#define BOOSTMSM_EVENTS_HPP_

namespace  // Concrete FSM implementation
{
    // Main Events
    struct sitting_button {};
    struct standing_up_button {};
    struct walking_button {};
    struct standing_button {};
    struct exercising_button {};
    struct turning_button {};
    struct sitting_down_button {};
    struct installation_button {};
    struct kill {};
    struct time_out {};
    // Sub-Events
    struct imu_detection {};
    struct imu_timeout {};
    struct first_step_completed {};
    struct finished {};
    // Errror events
    struct pause{};
    struct gentle_trap {};
    struct trap{};
    struct alert{};
}

#endif /* BOOSTMSM_EVENTS_HPP_ */
