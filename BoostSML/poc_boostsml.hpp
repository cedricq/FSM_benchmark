// $CXX -std=c++14 main_fsm.cpp
#include "sml.hpp"

#include "sml_logging.hpp"
#include "sml_2_uml.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>

namespace sml = boost::sml;

namespace {

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
// Sub-Events
struct imu_detection {};
struct imu_timeout {};
struct first_step_completed {};
struct last_step_completed {};
struct finished {};
// Errror events
struct gentle_trap {};
struct trap{};
struct alert{};

struct walk_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
       "FirstStep"_s  <= *"TriggerWalk"_s  + event<imu_detection>,
       "Walk"_s       <= "FirstStep"_s     + event<first_step_completed>,
       "LastStep"_s   <= "Walk"_s          + event<standing_button>,
       X              <= "LastStep"_s      + event<last_step_completed>
    );
  }
};


const auto is_chair_height_valid = [](const standing_up_button&) { std::cout<<"chair valid" <<std::endl; return true; };
const auto EnablingStops = [] {std::cout<<"stops enabled" <<std::endl;};
const auto DisablingStops = [] {std::cout<<"stops disabled" <<std::endl;};

struct main_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
      "Sitting"_s       <=*"Installation"_s   + event<sitting_button>    / EnablingStops,
      "Standing"_s      <= "Sitting"_s        + event<standing_up_button> [ is_chair_height_valid ],
      state<walk_fsm>   <= "Standing"_s       + event<walking_button>,
      "Turning"_s       <= "Standing"_s       + event<turning_button>,
      "Exercising"_s    <= "Standing"_s       + event<exercising_button>,
      "Sitting"_s       <= "Standing"_s       + event<sitting_down_button>,
      "Standing"_s      <= state<walk_fsm>    + event<finished>,
      "Standing"_s      <= "Turning"_s        + event<standing_button>,
      "Standing"_s      <= "Exercising"_s     + event<standing_button>,
      "Installation"_s  <= "Sitting"_s        + event<installation_button> / DisablingStops
    );
  }
};

struct overall_fsm {
  auto operator()() const {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        state<main_fsm>  <= *"NoError"_s,

        "GentleTrap"_s <= state<main_fsm> + event<gentle_trap> / [] { std::cout << "No Error -> Gentle Trap !!" << std::endl; },

        "Trap"_s       <= "main_fsm"_s    + event<trap> / [] { std::cout << "No Error -> Trap !!" << std::endl; },
        "Trap"_s       <= "GentleTrap"_s  + event<finished> / [] { std::cout << "Gentle Trap OK -> TRAP !!" << std::endl; },
        "Trap"_s       <= "GentleTrap"_s  + event<gentle_trap> / [] { std::cout << "Gentle Trap interrupted -> TRAP !!" << std::endl; },
        "Trap"_s       <= "GentleTrap"_s  + event<trap> / [] { std::cout << "Gentle Trap interrupted -> TRAP !!" << std::endl; },

        "Alert"_s      <= "NoError"_s     + event<alert> / [] { std::cout << "Alert !!" << std::endl; },
        "Alert"_s      <= "GentleTrap"_s  + event<alert> / [] { std::cout << "Alert !!" << std::endl; },
        "Alert"_s      <= "Trap"_s        + event<alert> / [] { std::cout << "Alert !!" << std::endl; },

        X              <= state<main_fsm> + event<kill>
    );
    // clang-format on
  }
};

}  // namespace

