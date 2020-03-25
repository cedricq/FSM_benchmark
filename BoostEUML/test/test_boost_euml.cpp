#include "gtest/gtest.h"

#include <fstream>
#include "../main_fsm.hpp"

namespace {

static std::string const state_names[] = { "Sitting", "Installation", "SittingDown", "StandingUp", "Walk", "Turn", "Standing"};

TEST(BoostEUML, MainFSM)
{
    main_fsm sm;
    sm.start();

    ASSERT_EQ("Installation", state_names[sm.current_state()[0]]);
    ASSERT_FALSE(sm.get_attribute(stopsEnabled));

    sm.process_event(sitting_button());
    ASSERT_EQ("Sitting", state_names[sm.current_state()[0]]);
    ASSERT_TRUE(sm.get_attribute(stopsEnabled));

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

    ASSERT_TRUE(sm.get_attribute(stopsEnabled));
    sm.process_event(installation_button());
    ASSERT_EQ("Installation", state_names[sm.current_state()[0]]);
    ASSERT_FALSE(sm.get_attribute(stopsEnabled));

    sm.process_event(sitting_button());
    ASSERT_EQ("Sitting", state_names[sm.current_state()[0]]);

    sm.stop();
    sm.start();
    ASSERT_EQ("Installation", state_names[sm.current_state()[0]]);
}

static std::string const wlk_state_names[] = { "TriggerWalk", "FirstStep", "Walking", "LastStep"};

TEST(BoostEUML, WalkFSM)
{
    Walk_type sm;
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

TEST(BoostEUML, OverallFSM)
{
    overall_fsm sm;
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

