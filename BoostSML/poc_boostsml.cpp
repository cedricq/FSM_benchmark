// $CXX -std=c++14 main_fsm.cpp
#include "sml.hpp"
#include <cassert>
#include <iostream>

namespace sml = boost::sml;

namespace {
struct sitting_button {};
struct standing_up_button {};
struct walking_button {};
struct standing_button {};
struct exercising_button {};
struct turning_button {};
struct sitting_down_button {};
struct installation_button {};
struct kill {};

const auto is_chair_height_valid = [](const standing_up_button&) { std::cout<<"chair valid" <<std::endl; return true; };

const auto EnablingStops = [] {std::cout<<"stops enabled" <<std::endl;};
const auto DisablingStops = [] {std::cout<<"stops disabled" <<std::endl;};

class Installation;
class Sitting;
class Standing;
class Walking;
class Turning;
class Exercising;

struct main_fsm {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(
      *state<Installation> 	+ event<sitting_button>	/ EnablingStops 				= state<Sitting>,
       state<Sitting> 		+ event<standing_up_button> [ is_chair_height_valid ] 	= state<Standing>,
       state<Standing> 		+ event<walking_button> 		                        = state<Walking>,
	   state<Standing> 		+ event<turning_button> 		                        = state<Turning>,
	   state<Standing> 		+ event<exercising_button> 		                        = state<Exercising>,
	   state<Standing> 		+ event<sitting_down_button> 	                        = state<Sitting>,
       state<Walking> 		+ event<standing_button> 		                        = state<Standing>,
	   state<Turning> 		+ event<standing_button> 		                        = state<Standing>,
	   state<Exercising> 	+ event<standing_button> 		                        = state<Standing>,
	   state<Sitting> 		+ event<installation_button> / DisablingStops 			= state<Installation>,
       state<Installation> 	+ event<kill> 											= X
    );
  }
};
}

int main() {
  using namespace sml;

  sm<main_fsm> sm;

  assert(sm.is(state<Installation>));

  sm.process_event(sitting_button{});
  assert(sm.is(state<Sitting>));

  sm.process_event(standing_up_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(walking_button{});
  assert(sm.is(state<Walking>));
  sm.process_event(standing_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(turning_button{});
  assert(sm.is(state<Turning>));
  sm.process_event(standing_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(exercising_button{});
  assert(sm.is(state<Exercising>));
  sm.process_event(standing_button{});
  assert(sm.is(state<Standing>));

  sm.process_event(sitting_down_button{});
  assert(sm.is(state<Sitting>));

  sm.process_event(installation_button{});
  assert(sm.is(state<Installation>));

}
