// $CXX -std=c++14 main_fsm.cpp
#include "sml.hpp"

#include <cstdio>
#include <cassert>
#include <iostream>
#include <string>
#include <typeinfo>

namespace sml = boost::sml;

// This log just helps debugging state machine transitions
namespace {
struct my_logger {
  template <class SM, class TEvent>
  void log_process_event(const TEvent&) {
    printf("[%s][process_event] %s\n", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TEvent>());
  }

  template <class SM, class TGuard, class TEvent>
  void log_guard(const TGuard&, const TEvent&, bool result) {
    printf("[%s][guard] %s %s %s\n", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TGuard>(),
           sml::aux::get_type_name<TEvent>(), (result ? "[OK]" : "[Reject]"));
  }

  template <class SM, class TAction, class TEvent>
  void log_action(const TAction&, const TEvent&) {
    printf("[%s][action] %s %s\n", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TAction>(),
           sml::aux::get_type_name<TEvent>());
  }

  template <class SM, class TSrcState, class TDstState>
  void log_state_change(const TSrcState& src, const TDstState& dst) {
    printf("[%s][transition] %s -> %s\n", sml::aux::get_type_name<SM>(), src.c_str(), dst.c_str());
  }
};
}

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

// Main States
// States
class Installation;
class Sitting;
class Standing;
class Turning;
class Exercising;
// Sub-States
class TriggerWalk;
class FirstStep;
class Walk;
class LastStep;

struct main_fsm;

struct walk_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
       state<FirstStep> <= *state<TriggerWalk>  + event<imu_detection>,
       state<Walk>      <= state<FirstStep>     + event<first_step_completed>,
       state<LastStep>  <= state<Walk>          + event<standing_button>,
       X                <= state<LastStep>      + event<last_step_completed>
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
      state<Sitting>       <=*state<Installation>   + event<sitting_button>    / EnablingStops,
      state<Standing>      <= state<Sitting>        + event<standing_up_button> [ is_chair_height_valid ],
      state<walk_fsm>      <= state<Standing>       + event<walking_button>,
      state<Turning>       <= state<Standing>       + event<turning_button>,
      state<Exercising>    <= state<Standing>       + event<exercising_button>,
      state<Sitting>       <= state<Standing>       + event<sitting_down_button>,
      state<Standing>      <= state<walk_fsm>       + event<finished>,
      state<Standing>      <= state<Turning>        + event<standing_button>,
      state<Standing>      <= state<Exercising>     + event<standing_button>,
      state<Installation>  <= state<Sitting>        + event<installation_button> / DisablingStops,
      X                    <= state<Installation>   + event<kill>
    );
  }
};

}

// This is to output state machine in Plant UML format
template <class T>
void dump_transition() noexcept {
  auto src_state = std::string{sml::aux::string<typename T::src_state>{}.c_str()};
  auto dst_state = std::string{sml::aux::string<typename T::dst_state>{}.c_str()};
  if (dst_state == "X") {
    dst_state = "[*]";
  }

  if (T::initial) {
    std::cout << "[*] --> " << src_state << std::endl;
  }

  std::cout << src_state << " --> " << dst_state;

  const auto has_event = !sml::aux::is_same<typename T::event, sml::anonymous>::value;
  const auto has_guard = !sml::aux::is_same<typename T::guard, sml::front::always>::value;
  const auto has_action = !sml::aux::is_same<typename T::action, sml::front::none>::value;

  if (has_event || has_guard || has_action) {
    std::cout << " :";
  }

  if (has_event) {
    std::cout << " " << boost::sml::aux::get_type_name<typename T::event>();
  }

  if (has_guard) {
    std::cout << " [" << boost::sml::aux::get_type_name<typename T::guard::type>() << "]";
  }

  if (has_action) {
    std::cout << " / " << boost::sml::aux::get_type_name<typename T::action::type>();
  }

  std::cout << std::endl;
}

template <template <class...> class T, class... Ts>
void dump_transitions(const T<Ts...>&) noexcept {
  int _[]{0, (dump_transition<Ts>(), 0)...};
  (void)_;
}

template <class SM>
void dump(const SM&) noexcept {
  std::cout << "@startuml" << std::endl << std::endl;
  dump_transitions(typename SM::transitions{});
  std::cout << std::endl << "@enduml" << std::endl;
}

int main() {
  using namespace sml;

  my_logger logger;
  sml::sm<main_fsm, sml::logger<my_logger>> sm{logger};

  assert(sm.is(state<Installation>));

  sm.process_event(sitting_button{});
  assert(sm.is(state<Sitting>));

  sm.process_event(standing_up_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(turning_button{});
  assert(sm.is(state<Turning>));
  sm.process_event(standing_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(exercising_button{});
  assert(sm.is(state<Exercising>));
  sm.process_event(standing_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(walking_button{});
  assert(sm.is(state<walk_fsm>));
  assert(sm.is<decltype(state<walk_fsm>)>(state<TriggerWalk>));

  sm.process_event(imu_detection{});
  assert(sm.is<decltype(state<walk_fsm>)>(state<FirstStep>));

  sm.process_event(first_step_completed{});
  assert(sm.is<decltype(state<walk_fsm>)>(state<Walk>));

  sm.process_event(standing_button{});
  assert(sm.is<decltype(state<walk_fsm>)>(state<LastStep>));

  sm.process_event(last_step_completed{});
  assert(sm.is<decltype(state<walk_fsm>)>(X));

  sm.process_event(finished{});
  assert(sm.is(state<Standing>));

  sm.process_event(sitting_down_button{});
  assert(sm.is(state<Sitting>));

  sm.process_event(installation_button{});
  assert(sm.is(state<Installation>));

  sm.process_event(kill{});
  assert(sm.is(X));

  // Export state machine plantuml
  std::cout<<std::endl;
  dump(sm);
}
