# State Machines design comparison

## TinyFSM

```
struct Installation : State
{
  void entry() override { std::cout << "* State is Installation" << std::endl; };
  void react(Toggle const &) override { transit<Sitting>(); };
};

struct Sitting : State
{
  void entry() override { std::cout << "* State is Sitting" << std::endl; };
  void react(Toggle const &) override { transit<Standing>(); };
};

struct Standing : State
{
  void entry() override { std::cout << "* State is Standing" << std::endl; };
  void react(Toggle const &) override { transit<Walking>(); };
};
...
```

## Boost.SML

```
struct main_fsm {
     auto operator()() const {
    using namespace sml;
    return make_transition_table(
        *state<Installation> + event<sitting_button>    / EnablingStops            = state<Sitting>,
        state<Sitting>       + event<standing_up_button> [ is_chair_height_valid ] = state<Standing>,
        state<Standing>      + event<walking_button>                               = state<Walking>,
        state<Standing>      + event<turning_button>                               = state<Turning>,
        state<Standing>      + event<exercising_button>                            = state<Exercising>,
        state<Standing>      + event<sitting_down_button>                          = state<Sitting>,
        state<Walking>       + event<standing_button>                              = state<Standing>,
        state<Turning>       + event<standing_button>                              = state<Standing>,
        state<Exercising>    + event<standing_button>                              = state<Standing>,
        state<Sitting>       + event<installation_button> / DisablingStops         = state<Installation>,
        state<Installation>  + event<kill>                                         = X
    );
  }
};
}
```
