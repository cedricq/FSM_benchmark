#include "gtest/gtest.h"

#include <fstream>
#include "../main_fsm.hpp"

namespace {

// Testing utilities.
static std::string const state_names[] = { "Installation", "Sitting", "StandingUp", "Standing", "SittingDown", "Walk" , "Turn"};

TEST(BoostMSM, MainFSM)
{
    fsm sm;
    sm.start();

    ASSERT_EQ("Installation", state_names[sm.current_state()[0]]);
    ASSERT_FALSE(sm.stopsEnabled);

    sm.process_event(sitting_button());
    ASSERT_EQ("Sitting", state_names[sm.current_state()[0]]);
    ASSERT_TRUE(sm.stopsEnabled);

    sm.process_event(standing_up_button());
    ASSERT_EQ("StandingUp", state_names[sm.current_state()[0]]);
    sm.process_event(finished());
    ASSERT_EQ("Standing", state_names[sm.current_state()[0]]);

    sm.process_event(walking_button());
    ASSERT_EQ("Walk", state_names[sm.current_state()[0]]);
    sm.process_event(imu_detection());
    sm.process_event(first_step_completed());
    sm.process_event(standing_button());
    sm.process_event(finished());
    ASSERT_EQ("Standing", state_names[sm.current_state()[0]]);

    sm.process_event(turning_button());
    ASSERT_EQ("Turn", state_names[sm.current_state()[0]]);
    sm.process_event(imu_detection());
    sm.process_event(first_step_completed());
    sm.process_event(standing_button());
    sm.process_event(finished());
    ASSERT_EQ("Standing", state_names[sm.current_state()[0]]);

    sm.process_event(sitting_down_button());
    ASSERT_EQ("SittingDown", state_names[sm.current_state()[0]]);
    sm.process_event(finished());
    ASSERT_EQ("Sitting", state_names[sm.current_state()[0]]);

    ASSERT_TRUE(sm.stopsEnabled);
    sm.process_event(installation_button());
    ASSERT_EQ("Installation", state_names[sm.current_state()[0]]);
    ASSERT_FALSE(sm.stopsEnabled);

    std::cout << "stop fsm" << std::endl;
    sm.stop();
    std::cout << "restart fsm" << std::endl;
    sm.start();
}

static std::string const wlk_state_names[] = { "TriggerWalk", "FirstStep", "Walking", "LastStep"};

TEST(BoostMSM, WalkFSM)
{
    wlk sm;
    sm.start();

    ASSERT_EQ("TriggerWalk", wlk_state_names[sm.current_state()[0]]);
    sm.process_event(imu_detection());
    ASSERT_EQ("FirstStep", wlk_state_names[sm.current_state()[0]]);
    sm.process_event(first_step_completed());
    ASSERT_EQ("Walking", wlk_state_names[sm.current_state()[0]]);
    sm.process_event(standing_button());
    ASSERT_EQ("LastStep", wlk_state_names[sm.current_state()[0]]);
}

}  // namespace

