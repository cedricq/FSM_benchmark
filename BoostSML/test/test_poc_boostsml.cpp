#ifndef BOOSTSML_POC_BOOSTSML_HPP_
#define BOOSTSML_POC_BOOSTSML_HPP_

#include "../poc_boostsml.hpp"

#include "gtest/gtest.h"

#include <fstream>

namespace {

TEST(BoostSML, MainFSM)
{
  using namespace sml;

  my_logger logger;
  sml::sm<main_fsm, sml::logger<my_logger>> sm{logger};

  ASSERT_TRUE(sm.is("Installation"_s));

  sm.process_event(sitting_button{});
  ASSERT_TRUE(sm.is("Sitting"_s));

  sm.process_event(standing_up_button{});
  ASSERT_TRUE(sm.is("StandingUp"_s));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is("Standing"_s));

  sm.process_event(turning_button{});
  ASSERT_TRUE(sm.is(state<turn_fsm>));
  ASSERT_TRUE(sm.is<decltype(state<turn_fsm>)>("TriggerTurn"_s));
  sm.process_event(imu_detection{});
  ASSERT_TRUE(sm.is<decltype(state<turn_fsm>)>("FirstStep"_s));
  sm.process_event(first_step_completed{});
  ASSERT_TRUE(sm.is<decltype(state<turn_fsm>)>("Turn"_s));
  sm.process_event(standing_button{});
  ASSERT_TRUE(sm.is<decltype(state<turn_fsm>)>("LastStep"_s));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is("Standing"_s));

  sm.process_event(exercising_button{});
  ASSERT_TRUE(sm.is(state<exercise_fsm>));
  ASSERT_TRUE(sm.is<decltype(state<exercise_fsm>)>("Exercising"_s));
  sm.process_event(standing_button{});
  ASSERT_TRUE(sm.is<decltype(state<exercise_fsm>)>(X));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is("Standing"_s));

  sm.process_event(walking_button{});
  ASSERT_TRUE(sm.is(state<walk_fsm>));
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("TriggerWalk"_s));
  sm.process_event(imu_detection{});
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("FirstStep"_s));
  sm.process_event(first_step_completed{});
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("Walk"_s));
  sm.process_event(standing_button{});
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("LastStep"_s));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is("Standing"_s));

  sm.process_event(sitting_down_button{});
  ASSERT_TRUE(sm.is("SittingDown"_s));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is("Sitting"_s));

  sm.process_event(installation_button{});
  ASSERT_TRUE(sm.is("Installation"_s));
}

TEST(BoostSML, ErrorFSM)
{
  using namespace sml;

  my_logger logger;
  sml::sm<error_fsm, sml::logger<my_logger>> sm{logger};

  ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Installation"_s));

  sm.process_event(sitting_button{});
  ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Sitting"_s));

  sm.process_event(standing_up_button{});
  ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("StandingUp"_s));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Standing"_s));

  sm.process_event(walking_button{});
  ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>(state<walk_fsm>));
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("TriggerWalk"_s));

  sm.process_event(imu_detection{});
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("FirstStep"_s));

  sm.process_event(first_step_completed{});
  ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("Walk"_s));


  sm.process_event(gentle_trap{});
  ASSERT_TRUE(sm.is("GentleTrap"_s));
  sm.process_event(finished{});
  ASSERT_TRUE(sm.is("Trap"_s));
  sm.process_event(alert{});
  ASSERT_TRUE(sm.is("Alert"_s));
}

TEST(BoostSML, KillingOverallFSM)
{
    using namespace sml;

    my_logger logger;
    sml::sm<overall_fsm, sml::logger<my_logger>> sm{logger};

    ASSERT_TRUE(sm.is("Initializing"_s));
    sm.process_event(finished{});
    ASSERT_TRUE(sm.is(state<error_fsm>));
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Installation"_s));

    sm.process_event(kill{});
    ASSERT_TRUE(sm.is("Closing"_s));
    sm.process_event(finished{});
    ASSERT_TRUE(sm.is(X));
}

TEST(BoostSML, PauseFromWalkFSM)
{
    using namespace sml;

    my_logger logger;
    sml::sm<error_fsm, sml::logger<my_logger>> sm{logger};

    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Installation"_s));

    sm.process_event(sitting_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Sitting"_s));

    sm.process_event(standing_up_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("StandingUp"_s));

    sm.process_event(finished{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Standing"_s));

    sm.process_event(walking_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>(state<walk_fsm>));
    ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("TriggerWalk"_s));

    sm.process_event(imu_detection{});
    ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("FirstStep"_s));

    sm.process_event(first_step_completed{});
    ASSERT_TRUE(sm.is<decltype(state<walk_fsm>)>("Walk"_s));

    sm.process_event(pause{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("RecoverPause"_s));

    sm.process_event(finished{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Pause"_s));

    sm.process_event(sitting_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Pause"_s));

    sm.process_event(standing_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Standing"_s));
}


TEST(BoostSML, PauseFromStandingUpFSM)
{
    using namespace sml;

    my_logger logger;
    sml::sm<error_fsm, sml::logger<my_logger>> sm{logger};

    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Installation"_s));

    sm.process_event(sitting_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Sitting"_s));

    sm.process_event(standing_up_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("StandingUp"_s));

    sm.process_event(pause{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Pause"_s));

    sm.process_event(sitting_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Pause"_s));

    sm.process_event(standing_button{});
    ASSERT_TRUE(sm.is<decltype(state<main_fsm>)>("Standing"_s));
}


TEST(BoostSML, MainStandingFSM_2_UML)
{
    using namespace sml;

    std::ofstream output;
    output.open("MainFSM_2_UML.txt");

    sml::sm<main_fsm> sm;
    dump(sm,output);
}

TEST(BoostSML, ErrorFSM_2_UML)
{
    using namespace sml;

    std::ofstream output;
    output.open("ErrorFSM_2_UML.txt");

    sml::sm<error_fsm> sm;
    dump(sm,output);
}

TEST(BoostSML, OverallFSM_2_UML)
{
    using namespace sml;

    std::ofstream output;
    output.open("OverallFSM_2_UML.txt");

    sml::sm<overall_fsm> sm;
    dump(sm,output);
}

}  // namespace

#endif
