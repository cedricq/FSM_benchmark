#include "gtest/gtest.h"

#include <fstream>
#include "../main_fsm.hpp"

namespace {

// Testing utilities.
static std::string const state_names[] = { "Installation", "Sitting", "StandingUp", "Standing", "SittingDown", "WalkFSM" , "TurnFSM"};

TEST(BoostMSM, MainFSM)
{
    MainFSM sm;
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
    ASSERT_EQ("WalkFSM", state_names[sm.current_state()[0]]);
    sm.process_event(imu_detection());
    sm.process_event(first_step_completed());
    sm.process_event(standing_button());
    sm.process_event(finished());
    ASSERT_EQ("Standing", state_names[sm.current_state()[0]]);

    sm.process_event(turning_button());
    ASSERT_EQ("TurnFSM", state_names[sm.current_state()[0]]);
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

    std::cout << "stop MainFSM" << std::endl;
    sm.stop();
    std::cout << "restart MainFSM" << std::endl;
    sm.start();
}

static std::string const wlk_state_names[] = { "TriggerWalk", "FirstStep", "Walking", "LastStep"};

TEST(BoostMSM, WalkFSM)
{
    WalkFSM sm;
    sm.start();

    ASSERT_EQ("TriggerWalk", wlk_state_names[sm.current_state()[0]]);
    sm.process_event(imu_detection());
    ASSERT_EQ("FirstStep", wlk_state_names[sm.current_state()[0]]);
    sm.process_event(first_step_completed());
    ASSERT_EQ("Walking", wlk_state_names[sm.current_state()[0]]);
    sm.process_event(standing_button());
    ASSERT_EQ("LastStep", wlk_state_names[sm.current_state()[0]]);
}

static std::string const overall_state_names[] = { "MainFSM", "GentleTrap", "Trap", "Initializing", "Running", "Closing", "Closed"};

TEST(BoostMSM, OverallFSM)
{
    OverallFSM sm;
    sm.start();

    sm.process_event(finished());
    //ASSERT_EQ("MainFSM", overall_state_names[sm.current_state()[0]]);
    sm.process_event(gentle_trap());
    //ASSERT_EQ("GentleTrap", overall_state_names[sm.current_state()[0]]);
    sm.process_event(trap());
    //ASSERT_EQ("Trap", overall_state_names[sm.current_state()[0]]);
    sm.process_event(alert());
    //ASSERT_EQ("Alert", overall_state_names[sm.current_state()[0]]);
    sm.process_event(kill());
    //ASSERT_EQ("Closing", overall_state_names[sm.current_state()[0]]);
    sm.process_event(finished());
    //ASSERT_EQ("Closed", overall_state_names[sm.current_state()[0]]);
}
}  // namespace

