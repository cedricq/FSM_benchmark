#ifndef MYFSM_HPP
#define MYFSM_HPP

#include <string>
#include <vector>
#include <functional>
#include <memory>

#include <iostream>

namespace my_fsm
{

using bool_t = bool;
    
#define ADD_TRANSITION(stateFrom, event, stateTo) stateFrom->AddTransition({event, stateTo, NoGuard, NoAction})
#define ADD_TRANSITION_GUARD(stateFrom, event, guard, stateTo) stateFrom->AddTransition({event, stateTo, guard, NoAction})
#define ADD_TRANSITION_ACTION(stateFrom, event, action, stateTo) stateFrom->AddTransition({event, stateTo, NoGuard, action})
#define ADD_TRANSITION_GUARD_ACTION(stateFrom, event, guard, action, stateTo) stateFrom->AddTransition({event, stateTo, guard, action})
#define SET_INIT_STATE(stateMachine, state) stateMachine->SetInitState(state)

#define SET_ON_ENTRY(state, function) state->SetOnEntry(function)
#define SET_ON_EXIT(state, function) state->SetOnExit(function)
#define SET_RUN(state, function) state->SetRun(function)

auto NoAction = []{};
auto NoGuard  = []{return true;};

class State_ ; // Forward declaration

struct EventId
{
    int id;
    std::string name;

    bool_t operator==(const EventId& event)
    {
        return ( this->id == event.id ) && ( this->name == event.name ) ;
    }

    bool_t operator!=(const EventId& event)
    {
        return not ( *this == event ) ;
    }
};

struct Transition
{
    EventId eventId;
    std::shared_ptr<State_> stateTo;
    std::function<bool_t()> guard;
    std::function<void()> action;
};

class State_
{
public:
    State_(std::string name) : name_(name) {};

    virtual ~State_() { transitionTable_.clear(); }

    void AddTransition(Transition t)
    {
        transitionTable_.push_back(t);
    }

    void SetRun(std::function<void()> f)
    {
        Run_ = f;
    }

    void SetOnEntry(std::function<void()> f)
    {
        OnEntry_ = f;
    }

    void SetOnExit(std::function<void()> f)
    {
        OnExit_ = f;
    }

    virtual std::shared_ptr<State_> HandleEvent(EventId const& e)
    {
        std::cout<<"[FSM] --> Event posted " <<e.name.c_str() <<" in " <<name_.c_str() <<std::endl;
        for (auto transition : transitionTable_)
        {
            if ( transition.eventId == e and transition.guard() )
            {
                transition.action();
                return transition.stateTo;
            }
        }
        std::cout<<"[FSM] --- No possible transitions from " <<name_.c_str() <<" on event " <<e.name.c_str() <<std::endl;
        return nullptr;
    }

    std::string GetName() { return name_; };

    std::function<void()> Run_     = NoAction;
    std::function<void()> OnEntry_ = NoAction;
    std::function<void()> OnExit_  = NoAction;

protected:

private:
    std::vector<Transition> transitionTable_ {};
    std::string name_;
};

using State = std::shared_ptr<State_>;

class StateManagerClass_ : public State_
{
public:
    StateManagerClass_(std::string name) : State_(name) {};

    virtual ~StateManagerClass_() {}

    virtual std::shared_ptr<State_> HandleEvent(EventId const& e) override
    {
        State newState(currentState_->HandleEvent(e));
        if (newState == nullptr)
        {
            newState = State_::HandleEvent(e);    
        }
        
        if (newState != nullptr)
        {
            std::cout<<"[FSM] <-- Transition complete from " <<currentState_->GetName().c_str() <<" to " <<newState->GetName().c_str() <<std::endl;
            currentState_->OnExit_();
            newState->OnEntry_();
            currentState_ = newState;
        }
        
        return newState;
    }

    void Start()
    {
        Reset();
        currentState_->OnEntry_();
    };

    void Run()
    {
        currentState_->Run_();
    }

    void Reset()
    {
        currentState_ = initState_ ;
    }

    void SetInitState(State state)
    {
        initState_ = state;
        Reset();
    }

    bool IsState(std::string const& str)
    {
        return str == currentState_->GetName();
    }

private:
    State initState_ {nullptr};
    State currentState_ {nullptr};

};

using StateManager = std::shared_ptr<StateManagerClass_>;

} // namespace


#endif /* MYFSM_HPP */
