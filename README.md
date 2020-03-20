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
```
