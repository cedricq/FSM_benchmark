#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#include <functional>
#include <memory>
#include <algorithm>

#include <vector>
//#include <string>
#include <stdio.h>

#include "NameString.h"

namespace FSM
{
////////////////////////////////////////////////////////////////////////////////////////////////
/// Macro helpers helping defining state machine
////////////////////////////////////////////////////////////////////////////////////////////////
#define ADD_TRANSITION(stateFrom, event, stateTo) stateFrom->addTransition({event, stateTo, NoGuard, NoAction})
#define ADD_TRANSITION_GUARD(stateFrom, event, guard, stateTo) stateFrom->addTransition({event, stateTo, guard, NoAction})
#define ADD_TRANSITION_ACTION(stateFrom, event, action, stateTo) stateFrom->addTransition({event, stateTo, NoGuard, action})
#define ADD_TRANSITION_GUARD_ACTION(stateFrom, event, guard, action, stateTo) stateFrom->addTransition({event, stateTo, guard, action})
#define SET_INIT_STATE(stateMachine, state) stateMachine->setInitState(state)

#define SET_ON_ENTRY(state, function) state->setOnEntry(function)
#define SET_ON_EXIT(state, function) state->setOnExit(function)
#define SET_RUN(state, function) state->setRun(function)

#define MAKE_SIMPLE_EVENT_MAPPING(stateEvent, simpleEvent)   FSM::EventId const stateEvent { simpleEvent.name() }
#define MAKE_BUTTON_EVENT_MAPPING(stateEvent, buttonEvent)   FSM::EventId const stateEvent { buttonEvent.name() }
#define MAKE_INTERNAL_EVENT(stateEvent)                      FSM::EventId const stateEvent { "internal_" #stateEvent }
#define EVENT(simpleEvent)                                   FSM::EventId{simpleEvent.name()}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Default empty guard and action.
////////////////////////////////////////////////////////////////////////////////////////////////
const auto NoAction = []() {};
const auto NoGuard  = []() {return true;};

// Forward declaration
class State_ ;


////////////////////////////////////////////////////////////////////////////////////////////////
/// Event structure in the context of a state machine
////////////////////////////////////////////////////////////////////////////////////////////////
struct EventId
{
    NameString name;

    bool operator==(EventId const& other) const
    {
        return (name == other.name);
    }

    bool operator!=(EventId const& other) const
    {
        return not (*this == other);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////
/// Transition structure in the context of a state machine
////////////////////////////////////////////////////////////////////////////////////////////////
struct Transition
{
    EventId eventId;
    std::shared_ptr<State_> stateTo;
    std::function<bool()> guard;
    std::function<void()> action;
};

////////////////////////////////////////////////////////////////////////////////////////////////
/// Mother class of all states
////////////////////////////////////////////////////////////////////////////////////////////////

class State_ : public std::enable_shared_from_this<State_>
{
public:
    State_(NameString const& name) : name_(name) {};

    virtual ~State_();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Add a new transition in the vector
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void addTransition(Transition t);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Set the run method which can be called anywhere else
    ///             by the owner of the state machine (e.g in a tick)
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setRun(std::function<void()> f);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Set the On Entry method which is called when entering the state
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setOnEntry(std::function<void()> f);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Set the On Exit method which is called when exiting the state
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setOnExit(std::function<void()> f);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Checks if a possible transition exists and if so returns the State To
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<State_> handleEvent(EventId const& e);

    /// \brief      Getters
    virtual NameString const& name();
    virtual std::shared_ptr<State_> getState();

    /// \brief      Executors
    virtual void run();
    virtual void onEntry();
    virtual void onExit();

    virtual bool isStateManager();
    virtual std::shared_ptr<State_> getInitState();

    std::function<void()> run_     = NoAction;
    std::function<void()> onEntry_ = NoAction;
    std::function<void()> onExit_  = NoAction;

private:
    std::vector<Transition> transitionTable_ {};
    NameString name_;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Public accessor of State_ shared pointer
////////////////////////////////////////////////////////////////////////////////////////////////
using State = std::shared_ptr<State_>;

// State which indicates that a transition has already occurred (one transition max by event)
extern State const StateTransitionAlreadyComplete;

// State which indicates that a state machine is complete and can return if possible
extern State const Exit;


char_t constexpr FSM_DATA_ACCESSOR[] = "StateMachine";

// Access to FSM state value by channel of choice.
// For now in SHM
class AbstractExposedStateStrategy
{
public:
    AbstractExposedStateStrategy() = default;
    virtual ~AbstractExposedStateStrategy() = default;

    virtual void update(NameString const& state) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////
/// Mother class of all state manager handling transitions and which is a State_ as well
////////////////////////////////////////////////////////////////////////////////////////////////
class StateManagerClass_ : public State_
{
public:
    StateManagerClass_(NameString const& name, bool isVerbose = true)
        : State_(name)
        , isVerbose_(isVerbose)
        {};

    virtual ~StateManagerClass_() = default;

    void start();

    void run() override;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Checks if in the current state a possible transition exists
    ///             and if so changes the current state
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<State_> handleEvent(EventId const& e);

    /// \brief      Executors
    virtual void onEntry();
    virtual void onExit();
    virtual void reset();

    /// \brief      Set entry state of this state manager
    virtual void setInitState(State state);

    /// \brief      Utilities
    bool isState(NameString const& str);
    bool isState(char_t const* str);
    NameString const& name() override;
    State getState() override;
    State getInitState() override;
    bool isStateManager() override;

    void addExposedStrategy(std::shared_ptr<AbstractExposedStateStrategy> strategy);
    std::vector<std::shared_ptr<AbstractExposedStateStrategy>> const& exposedStrategies() const { return exposedStrategies_; }
    void updateState();

protected:
    State initState_ {nullptr};
    State currentState_ {nullptr};
    std::vector<std::shared_ptr<AbstractExposedStateStrategy>> exposedStrategies_{};

private:
    bool isVerbose_;
};


////////////////////////////////////////////////////////////////////////////////////////////////
// Public accessor of StateManagerClass_ shared pointer
////////////////////////////////////////////////////////////////////////////////////////////////
using StateManager = std::shared_ptr<StateManagerClass_>;

} // namespace




#endif
