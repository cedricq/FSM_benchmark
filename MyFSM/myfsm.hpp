#ifndef CORE_STATEMACHINE_H_
#define CORE_STATEMACHINE_H_

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include <iostream>
#include <stdio.h>

//#include "wdc/core/Logger.h"

#define bool_t bool

namespace core
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

#define MAKE_EVENT_MAPPING(stateEvent, systemEvent, str) EventId const stateEvent { static_cast<int>(systemEvent), static_cast<std::string>(str).c_str()}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Default empty guard and action.
////////////////////////////////////////////////////////////////////////////////////////////////
auto NoAction = []() {};
auto NoGuard  = []() {return true;};

// Forward declaration
class State_ ;

////////////////////////////////////////////////////////////////////////////////////////////////
/// Event structure in the context of a state machine
////////////////////////////////////////////////////////////////////////////////////////////////
struct EventId
{
    int id;
    std::string name;

    bool_t operator==(EventId const& event)
    {
        return ( this->id == event.id ) && ( this->name == event.name ) ;
    }

    bool_t operator!=(EventId const& event)
    {
        return not ( *this == event ) ;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////
/// Transition structure in the context of a state machine
////////////////////////////////////////////////////////////////////////////////////////////////
struct Transition
{
    EventId eventId;
    std::shared_ptr<State_> stateTo;
    std::function<bool_t()> guard;
    std::function<void()> action;
};

////////////////////////////////////////////////////////////////////////////////////////////////
/// Mother class of all states
////////////////////////////////////////////////////////////////////////////////////////////////
class State_ : public std::enable_shared_from_this<State_>
{
public:
    State_(std::string const& name) : name_(name) {};

    virtual ~State_() { transitionTable_.clear(); }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Add a new transition in the vector
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void addTransition(Transition t)
    {
        transitionTable_.push_back(t);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Set the run method which can be called anywhere else
    ///             by the owner of the state machine (e.g in a tick)
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setRun(std::function<void()> f)
    {
        run_ = f;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Set the On Entry method which is called when entering the state
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setOnEntry(std::function<void()> f)
    {
        onEntry_ = f;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Set the On Exit method which is called when exiting the state
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void setOnExit(std::function<void()> f)
    {
        onExit_ = f;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Checks if a possible transition exists and if so returns the State To
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<State_> handleEvent(EventId const& e)
    {
        for (auto transition : transitionTable_)
        {
            if ( transition.eventId.id == e.id and transition.guard() )
            {
                transition.action();
                return transition.stateTo;
            }
        }
        return nullptr;
    }

    virtual std::string const& getName() { return name_; };

    virtual std::shared_ptr<State_> getState()
    {
        return shared_from_this();
    }

    virtual void run()
    {
        run_();
    }

    virtual void onEntry()
    {
        onEntry_();
    }

    virtual void onExit()
    {
        onExit_();
    }

    std::function<void()> run_     = NoAction;
    std::function<void()> onEntry_ = NoAction;
    std::function<void()> onExit_  = NoAction;

private:
    std::vector<Transition> transitionTable_ {};
    std::string name_;
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Public accessor of State_ shared pointer
////////////////////////////////////////////////////////////////////////////////////////////////
using State = std::shared_ptr<State_>;

void displayTransition(std::string const& managerName, std::string const& eventName, std::string const& currentStateName, std::string const& newStateName)
{
    //core::Logger::info("[FSM][%s] %s : %s --> %s", managerName().c_str(), eventName.name.c_str(), currentStateName.c_str(), newStateName.c_str());
    std::cout <<"[FSM][" <<managerName.c_str() <<"] " <<eventName.c_str() <<" : " <<currentStateName.c_str() <<" --> " <<newStateName.c_str() <<std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////
/// Mother class of all state manager handling transitions and which is a State_ as well
////////////////////////////////////////////////////////////////////////////////////////////////
class StateManagerClass_ : public State_
{
public:
    StateManagerClass_(std::string name) : State_(name) {};

    virtual ~StateManagerClass_() {}

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief      Checks if in the current state a possible transition exists
    ///             and if so changes the current state
    ////////////////////////////////////////////////////////////////////////////////////////////////
    virtual std::shared_ptr<State_> handleEvent(EventId const& e) override
    {
        EventId currentEvent = e;
        State newState(currentState_->handleEvent(currentEvent));

        if (newState != nullptr)
        {
            displayTransition(getName(), currentEvent.name, currentState_->getName(), newState->getName());
            currentState_->onExit();
            newState->onEntry();
            currentState_ = newState;
        }
        else
        {
            newState = State_::handleEvent(currentEvent);
            return newState;
        }
        return nullptr;
    }

    void start()
    {
        reset();
        currentState_->onEntry_();
    };

    void run() override
    {
        currentState_->run();
    }

    void onEntry() override
    {
        onEntry_();
        currentState_->onEntry();
    }

    void onExit() override
    {
        currentState_->onExit();
        onExit_();
    }

    void reset()
    {
        currentState_ = initState_ ;
    }

    void setInitState(State state)
    {
        initState_ = state;
        reset();
    }

    bool isState(std::string const& str)
    {
        return str == currentState_->getName();
    }

    std::string const& getName() override
    {
        return currentState_->getName();
    }

    State getState() override
    {
        return currentState_->getState();
    }

private:
    State initState_ {nullptr};
    State currentState_ {nullptr};

};

////////////////////////////////////////////////////////////////////////////////////////////////
// Public accessor of StateManagerClass_ shared pointer
////////////////////////////////////////////////////////////////////////////////////////////////
using StateManager = std::shared_ptr<StateManagerClass_>;

} // namespace




#endif
