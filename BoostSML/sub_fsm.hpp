#ifndef SUB_FSM_HPP
#define SUB_FSM_HPP

#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>
#include "include/sml.hpp"

namespace sml = boost::sml;

namespace {

struct walk_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
       "FirstStep"_s  <= *"TriggerWalk"_s  + event<imu_detection>,
       "Walk"_s       <= "FirstStep"_s     + event<first_step_completed>,
       "LastStep"_s   <= "Walk"_s          + event<standing_button>
    );
  }
};

struct turn_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
       "FirstStep"_s  <= *"TriggerTurn"_s  + event<imu_detection>,
       "Turn"_s       <= "FirstStep"_s     + event<first_step_completed>,
       "LastStep"_s   <= "Turn"_s          + event<standing_button>
    );
  }
};

struct exercise_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
       X  <= *"Exercising"_s   + event<standing_button>
    );
  }
};

}  // namespace
#endif
