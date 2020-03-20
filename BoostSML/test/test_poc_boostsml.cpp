#ifndef BOOSTSML_POC_BOOSTSML_HPP_
#define BOOSTSML_POC_BOOSTSML_HPP_

#include "../poc_boostsml.hpp"

void test_main_fsm(sml::sm<main_fsm, sml::logger<my_logger>>& sm)
{
  using namespace sml;

  assert(sm.is("Installation"_s));

  sm.process_event(sitting_button{});
  assert(sm.is("Sitting"_s));

  sm.process_event(standing_up_button{});
  assert(sm.is("Standing"_s));

  sm.process_event(turning_button{});
  assert(sm.is("Turning"_s));
  sm.process_event(standing_button{});
  assert(sm.is("Standing"_s));

  sm.process_event(exercising_button{});
  assert(sm.is("Exercising"_s));
  sm.process_event(standing_button{});
  assert(sm.is("Standing"_s));

  sm.process_event(walking_button{});
  assert(sm.is(state<walk_fsm>));
  assert(sm.is<decltype(state<walk_fsm>)>("TriggerWalk"_s));

  sm.process_event(imu_detection{});
  assert(sm.is<decltype(state<walk_fsm>)>("FirstStep"_s));

  sm.process_event(first_step_completed{});
  assert(sm.is<decltype(state<walk_fsm>)>("Walk"_s));

  sm.process_event(standing_button{});
  assert(sm.is<decltype(state<walk_fsm>)>("LastStep"_s));

  sm.process_event(last_step_completed{});
  assert(sm.is<decltype(state<walk_fsm>)>(X));

  sm.process_event(finished{});
  assert(sm.is("Standing"_s));

  sm.process_event(sitting_down_button{});
  assert(sm.is("Sitting"_s));

  sm.process_event(installation_button{});
  assert(sm.is("Installation"_s));

}

void test_overall_fsm(sml::sm<overall_fsm, sml::logger<my_logger>>& sm)
{
  using namespace sml;

  assert(sm.is<decltype(state<main_fsm>)>("Installation"_s));

  sm.process_event(sitting_button{});
  assert(sm.is<decltype(state<main_fsm>)>("Sitting"_s));

  sm.process_event(standing_up_button{});
  assert(sm.is<decltype(state<main_fsm>)>("Standing"_s));

  sm.process_event(walking_button{});
  assert(sm.is<decltype(state<main_fsm>)>(state<walk_fsm>));
  assert(sm.is<decltype(state<walk_fsm>)>("TriggerWalk"_s));

  sm.process_event(imu_detection{});
  assert(sm.is<decltype(state<walk_fsm>)>("FirstStep"_s));

  sm.process_event(first_step_completed{});
  assert(sm.is<decltype(state<walk_fsm>)>("Walk"_s));


  sm.process_event(gentle_trap{});
  assert(sm.is("GentleTrap"_s));
  sm.process_event(finished{});
  assert(sm.is("Trap"_s));
  sm.process_event(alert{});
  assert(sm.is("Alert"_s));

}

void test_kill_overall_fsm(sml::sm<overall_fsm, sml::logger<my_logger>>& sm)
{
    using namespace sml;

    assert(sm.is<decltype(state<main_fsm>)>("Installation"_s));

    sm.process_event(kill{});
    assert(sm.is(X));
}

int main() {

  using namespace sml;

  my_logger logger;
  sml::sm<main_fsm, sml::logger<my_logger>> main_sm{logger};

  test_main_fsm(main_sm);

  sml::sm<overall_fsm, sml::logger<my_logger>> overall_sm{logger};
  test_overall_fsm(overall_sm);

  sml::sm<overall_fsm, sml::logger<my_logger>> killed_overall_sm{logger};
  test_kill_overall_fsm(killed_overall_sm);

  // Export state machine plantuml
  std::cout<<std::endl;
  dump(main_sm);

  std::cout<<std::endl;
  dump(overall_sm);
}

#endif
