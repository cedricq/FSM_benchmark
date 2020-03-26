# State Machines design comparison

## Benchmark tables

| Subject                | SML   | MSM  | MSM-EUML |
| ---------------------- | ----- | ---- | -------- |
| Readability            | +++   | +    | ++       |
| Testability            | +++   | ++   | ++       |
| Complex state machines | ++(+) | +++  | +++      |
| Documention available  | +     | ++   | ++       |
| Maturity               | +     | ++   | ++       |
| Library volume         | +++   | +    | +        |
| Export FSM             | ++    | NA   | NA       |



| GCC-5.2          | [Boost].SML | [Boost.MSM-eUML] |
| ---------------- | ----------- | ---------------- |
| Compilation time | 0.175s      | 7.879s           |
| Execution time   | 15ms        | 19ms             |
| Memory usage     | 1b          | 32b              |



## Boost.SML

```c++
// Actions
auto EnablingStops = [] {stopsEnabled = true; std::cout<<"stops are enabled" <<std::endl;};
auto DisablingStops = [] {stopsEnabled = false; std::cout<<"stops are disabled" <<std::endl;};

// Guards
auto is_chair_height_valid = [](const auto&) { std::cout<<"chair height is valid" <<std::endl; return true; };
auto is_vertical = [](const auto&) { std::cout<<"exo is vertical" <<std::endl; return true; };

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
struct Installation : public msm::front::state<>
{
    // every (optional) entry/exit methods get the event passed.
    template <class Event,class FSM>
    void on_entry(Event const&,FSM& ) {std::cout << "entering: Installation" << std::endl;}
    template <class Event,class FSM>
    void on_exit(Event const&,FSM& ) {std::cout << "leaving: Installation" << std::endl;}
};
...
// transition actions
void EnableStops(sitting_button const&)       { stopsEnabled = true; }
void DisableStops(installation_button const&) { stopsEnabled = false; }

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
//    +------------+----------------------+---------------+-------------------+---------+
a_row < TriggerWalk, imu_detection        , FirstStep     , &wlk::StartWalking >,
_row  < FirstStep  , first_step_completed , Walking             >,
a_row < Walking    , standing_button      , LastStep      , &wlk::StopWalking  >
//    +------------+----------------------+---------------+--------------------+--------+
> {};
```



## Boost.eUML

```c++
//states
BOOST_MSM_EUML_STATE((installation_entry, installation_exit, attributes_ << stopsEnabled),Installation)
BOOST_MSM_EUML_STATE((),Sitting)
BOOST_MSM_EUML_STATE((),StandingUp)
BOOST_MSM_EUML_STATE((),Standing)
BOOST_MSM_EUML_STATE((),SittingDown)

// attributes
BOOST_MSM_EUML_DECLARE_ATTRIBUTE(bool, stopsEnabled)
    
// Actions
BOOST_MSM_EUML_ACTION(EnableStops)
{
    template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
    {
        std::cout<<"ACTION - Enable stops" <<std::endl;
    }
};

BOOST_MSM_EUML_ACTION(DisableStops)
{
    template <class FSM,class EVT,class SourceState,class TargetState>
        void operator()(EVT const& evt,FSM&,SourceState& ,TargetState& )
    {
        std::cout<<"ACTION - Disable stops" <<std::endl;
    }
};

BOOST_MSM_EUML_TRANSITION_TABLE((
    Installation == Sitting         + installation_button  / DisableStops,
//  +------------------------------------------------------------------------------+
    Sitting      == Installation    + sitting_button [not is_vertical]  / EnableStops,
    Sitting      == SittingDown     + finished ,
//  +------------------------------------------------------------------------------+
    StandingUp   == Sitting         + standing_up_button [is_chair_height_valid],
//  +------------------------------------------------------------------------------+
    Standing     == StandingUp      + finished ,
    Standing     == Walk            + finished ,
    Standing     == Turn            + finished ,
//  +------------------------------------------------------------------------------+
    SittingDown  == Standing        + sitting_down_button ,
//  +------------------------------------------------------------------------------+
    Walk         == Standing        + walking_button ,
//  +------------------------------------------------------------------------------+
    Turn         == Standing        + turning_button
//  +------------------------------------------------------------------------------+
), main_transition_table)
```

```c++
BOOST_MSM_EUML_TRANSITION_TABLE((
//  +------------------------------------------------------------------------------+
    FirstStep   == TriggerWalk       + imu_detection ,
    Walking     == FirstStep         + first_step_completed ,
    LastStep    == Walking           + standing_button
//  +------------------------------------------------------------------------------+
        ), walking_transition_table )
```



## TinyFSM

Just for fun as a 1st try.  We will not pursue with this library for the following reasons:

- Transitions defined for each step individually
- No general overview of FSM
- No compile time check

```c++
struct Toggle : tinyfsm::Event { };

struct State : tinyfsm::Fsm<State>
{
  static void reset(void);

  virtual void react(Toggle const &) { };

  // alternative: enforce handling of Toggle in all states (pure virtual)
  //virtual void react(Toggle const &) = 0;

  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void) { };  /* no exit actions */

  // alternative: enforce entry actions in all states (pure virtual)
  //virtual void entry(void) = 0;
};

void State::reset() {
  tinyfsm::StateList<Installation, Sitting, Standing, Walking, Exercising, Turning>::reset();
}

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
FSM_INITIAL_STATE(State, Installation)

    
```



