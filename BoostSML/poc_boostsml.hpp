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
struct time_out {};
// Sub-Events
struct imu_detection {};
struct imu_timeout {};
struct first_step_completed {};
struct last_step_completed {};
struct finished {};
// Errror events
struct pause{};
struct gentle_trap {};
struct trap{};
struct alert{};


// Actions
const auto EnablingStops = [] {std::cout<<"stops are enabled" <<std::endl;};
const auto DisablingStops = [] {std::cout<<"stops are disabled" <<std::endl;};

// Guards
const auto is_chair_height_valid = [](const auto&) { std::cout<<"chair height is valid" <<std::endl; return true; };
const auto is_vertical = [](const auto&) { std::cout<<"exo is vertical" <<std::endl; return true; };

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

struct turn_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
       "FirstStep"_s  <= *"TriggerTurn"_s  + event<imu_detection>,
       "Turn"_s       <= "FirstStep"_s     + event<first_step_completed>,
       "LastStep"_s   <= "Turn"_s          + event<standing_button>,
       X              <= "LastStep"_s      + event<last_step_completed>
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

struct main_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
        "Sitting"_s      <= *"Installation"_s  + event<sitting_button> / EnablingStops,
        "Installation"_s <= "Sitting"_s        + event<installation_button> / DisablingStops,

        "StandingUp"_s   <= "Sitting"_s        + event<standing_up_button> [ is_chair_height_valid ],
        "Standing"_s     <= "StandingUp"_s     + event<finished>,

        "SittingDown"_s   <= "Standing"_s      + event<sitting_down_button>,
        "Sitting"_s     <= "SittingDown"_s     + event<finished>,

        state<walk_fsm>  <= "Standing"_s       + event<walking_button>,
        "Standing"_s     <= state<walk_fsm>    + event<finished>,
        "Pause"_s        <= state<walk_fsm>    + event<pause>,

        state<turn_fsm>  <= "Standing"_s       + event<turning_button>,
        "Standing"_s     <= state<turn_fsm>    + event<finished>,
        "Pause"_s        <= state<turn_fsm>    + event<pause>,


        state<exercise_fsm> <= "Standing"_s        + event<exercising_button>,
        "Standing"_s        <= state<exercise_fsm> + event<finished>,
        "Pause"_s           <= state<exercise_fsm> + event<pause>,

        "Standing"_s     <=  "Pause"_s         + event<standing_button> [ is_vertical ],
        "Sitting"_s      <=  "Pause"_s         + event<sitting_button> [ !is_vertical ]
    );
  }
};

struct error_fsm {
  auto operator()() const {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        state<main_fsm> <= *"Running"_s / DisablingStops,

        "GentleTrap"_s  <= state<main_fsm> + event<gentle_trap> / [] { std::cout << "No Error -> Gentle Trap !!" << std::endl; },

        "Trap"_s        <= state<main_fsm> + event<trap> / [] { std::cout << "No Error -> Trap !!" << std::endl; },
        "Trap"_s        <= "GentleTrap"_s  + event<finished> / [] { std::cout << "Gentle Trap finished -> TRAP !!" << std::endl; },
        "Trap"_s        <= "GentleTrap"_s  + event<gentle_trap> / [] { std::cout << "re-Gentle Trap -> TRAP !!" << std::endl; },
        "Trap"_s        <= "GentleTrap"_s  + event<trap> / [] { std::cout << "Gentle Trap trapped -> TRAP !!" << std::endl; },
        "Trap"_s        <= "GentleTrap"_s  + event<time_out> / [] { std::cout << "Gentle Trap timed out -> TRAP !!" << std::endl; },

        "Alert"_s       <= "Running"_s     + event<alert> / [] { std::cout << "Alert !!" << std::endl; },
        "Alert"_s       <= "GentleTrap"_s  + event<alert> / [] { std::cout << "Alert !!" << std::endl; },
        "Alert"_s       <= "Trap"_s        + event<alert> / [] { std::cout << "Alert !!" << std::endl; }
    );
  }
};

struct overall_fsm {
  auto operator()() const {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        state<error_fsm> <= *"Initializing"_s + event<finished>,
        "Closing"_s      <= state<error_fsm>  + event<kill>,
        X                <= "Closing"_s       + event<finished>
    );
  }
};

}  // namespace

