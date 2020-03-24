# State Machines design comparison

## TinyFSM

```c++
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

```c++
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

```c++
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
```



## Boost.MSM

```c++
 struct transition_table : mpl::vector<
            //      Start            Event                   Next                  Action               Guard
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            a_row < Installation  , sitting_button        , Sitting           , &m::EnableStops         >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            a_row < Sitting       , installation_button   , Installation      , &m::DisableStops        >,
            _row <  Sitting       , standing_up_button    , StandingUp        >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < StandingUp     , finished              , Standing          >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Standing       , sitting_down_button   , SittingDown       >,
            _row < Standing       , walking_button        , Walk              >,
            _row < Standing       , turning_button        , Turn              >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < SittingDown    , finished              , Sitting           >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Walk           , finished              , Standing          >,
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
            _row < Turn           , finished              , Standing          >
            //    +---------------+-----------------------+-------------------+---------------------+----------------------+
        > {};

```

```c++
struct transition_table : mpl::vector3<
        //      Start       Event                    Next             Action               Guard
        //    +------------+----------------------+---------------+-----------------------+---------+
        a_row < TriggerWalk, imu_detection        , FirstStep     , &wlk::StartWalking >,
        _row  < FirstStep  , first_step_completed , Walking             >,
        a_row < Walking    , standing_button      , LastStep      , &wlk::StopWalking  >
        //    +------------+----------------------+---------------+------------------------+--------+
    > {};
```

