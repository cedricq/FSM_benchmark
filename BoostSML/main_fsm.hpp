#ifndef MAIN_FSM_HPP
#define MAIN_FSM_HPP

#include "events.hpp"
#include "sub_fsm.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>
#include "include/sml.hpp"

namespace sml = boost::sml;

namespace {

// Actions
const auto EnablingStops = [] {std::cout<<"stops are enabled" <<std::endl;};
const auto DisablingStops = [] {std::cout<<"stops are disabled" <<std::endl;};

// Guards
const auto is_chair_height_valid = [](const auto&) { std::cout<<"chair height is valid" <<std::endl; return true; };
const auto is_vertical = [](const auto&) { std::cout<<"exo is vertical" <<std::endl; return true; };

struct main_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
        "Sitting"_s         <= *"Installation"_s   + event<sitting_button> / EnablingStops,
        "Installation"_s    <= "Sitting"_s         + event<installation_button> / DisablingStops,

        "StandingUp"_s      <= "Sitting"_s         + event<standing_up_button> [ is_chair_height_valid ],
        "Standing"_s        <= "StandingUp"_s      + event<finished>,
        "Pause"_s           <= "StandingUp"_s      + event<pause>,

        "SittingDown"_s     <= "Standing"_s        + event<sitting_down_button>,
        "Sitting"_s         <= "SittingDown"_s     + event<finished>,
        "Pause"_s           <= "SittingDown"_s     + event<pause>,

        state<walk_fsm>     <= "Standing"_s        + event<walking_button>,
        "Standing"_s        <= state<walk_fsm>     + event<finished>,
        "RecoverPause"_s    <= state<walk_fsm>     + event<pause>,

        state<turn_fsm>     <= "Standing"_s        + event<turning_button>,
        "Standing"_s        <= state<turn_fsm>     + event<finished>,
        "RecoverPause"_s    <= state<turn_fsm>     + event<pause>,


        state<exercise_fsm> <= "Standing"_s        + event<exercising_button>,
        "Standing"_s        <= state<exercise_fsm> + event<finished>,
        "RecoverPause"_s    <= state<exercise_fsm> + event<pause>,

        "Standing"_s        <=  "Pause"_s          + event<standing_button> [ is_vertical ],
        "Sitting"_s         <=  "Pause"_s          + event<sitting_button> [ !is_vertical ],

        "Pause"_s           <=  "RecoverPause"_s   + event<finished>
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
#endif
