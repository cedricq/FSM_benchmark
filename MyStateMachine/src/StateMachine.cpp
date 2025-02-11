#include "StateMachine.h"

namespace FSM
{
    State const StateTransitionAlreadyComplete = std::make_shared<State_>("StateTransitionAlreadyComplete");
    State const Exit = std::make_shared<State_>("EXIT");

    State_::~State_()
    {
        transitionTable_.clear();
    }


    void State_::addTransition(Transition t)
    {
        transitionTable_.push_back(t);
    }


    void State_::setRun(std::function<void()> f)
    {
        run_ = f;
    }


    void State_::setOnEntry(std::function<void()> f)
    {
        onEntry_ = f;
    }

    void State_::setOnExit(std::function<void()> f)
    {
        onExit_ = f;
    }

    std::shared_ptr<State_> State_::handleEvent(EventId const& e)
    {
        for (auto& transition : transitionTable_)
        {
            if ( transition.eventId == e and transition.guard() )
            {
                transition.action();
                return transition.stateTo;
            }
        }
        return nullptr;
    }

    NameString const& State_::name()
    {
        return name_;
    }

    std::shared_ptr<State_> State_::getState()
    {
        return shared_from_this();
    }

    std::shared_ptr<State_> State_::getInitState()
    {
        return shared_from_this();
    }

    void State_::run()
    {
        run_();
    }

    void State_::onEntry()
    {
        onEntry_();
    }

    void State_::onExit()
    {
        onExit_();
    }

    bool State_::isStateManager()
    {
        return false;
    }


    void StateManagerClass_::start()
    {
        reset();
        currentState_->onEntry();
        updateState();
    }

    void StateManagerClass_::run()
    {
        currentState_->run();
    }

    std::shared_ptr<State_> StateManagerClass_::handleEvent(EventId const& e)
    {
        EventId currentEvent = e;
        State newState = currentState_->handleEvent(currentEvent);

        // Exiting capability allow a substate manager to exit and go back to the above manager
        // without the substate manager knowing where to
        if (newState == Exit)
        {
            if (currentState_->isStateManager())
            {
                NameString exitState = name();
                onEntry();
                if (isVerbose_)
                {
                    printf("[FSM][%s] %s --> %s\n", exitState.c_str(), currentEvent.name.c_str(), initState_->name().c_str());
                }
                return StateTransitionAlreadyComplete;
            }
            else
            {
                onExit();
                return Exit;
            }
        }
        // Transition identified
        else if (newState != nullptr and newState != StateTransitionAlreadyComplete)
        {
            if (newState != currentState_)
            {
                currentState_->onExit();
                NameString exitState = name();
                currentState_ = newState;
                currentState_->onEntry();
                if (isVerbose_)
                {
                    printf("[FSM][%s] %s --> %s\n", exitState.c_str(), currentEvent.name.c_str(), currentState_->name().c_str());
                }
                updateState();
            }
            return StateTransitionAlreadyComplete;
        }
        // Transition not identified at state level, check the state manager possible transitions
        else if (newState != StateTransitionAlreadyComplete)
        {
            newState = State_::handleEvent(currentEvent);
            return newState;
        }
        return nullptr;
    }


    void StateManagerClass_::onEntry()
    {
        onEntry_();
        currentState_->onEntry();
        updateState();
    }

    void StateManagerClass_::onExit()
    {
        currentState_->onExit();
        onExit_();
        updateState();
    }

    void StateManagerClass_::reset()
    {
        currentState_ = initState_ ;
    }

    void StateManagerClass_::setInitState(State state)
    {
        initState_ = state;
        reset();
    }

    bool StateManagerClass_::isState(NameString const& str)
    {
        return str == currentState_->name();
    }
    bool StateManagerClass_::isState(char_t const* str)
    {
    	return NameString(str) == currentState_->name();
    }

    NameString const& StateManagerClass_::name()
    {
        return currentState_->name();
    }

    State StateManagerClass_::getState()
    {
        return currentState_->getState();
    }

    State StateManagerClass_::getInitState()
    {
        return initState_;
    }

    bool StateManagerClass_::isStateManager()
    {
        return true;
    }

    void StateManagerClass_::addExposedStrategy(std::shared_ptr<AbstractExposedStateStrategy> exposedStrategy)
    {
        exposedStrategies_.emplace_back(exposedStrategy);
    }

    void StateManagerClass_::updateState()
    {
        for (auto& exposedState : exposedStrategies_)
        {
            exposedState->update(currentState_->name());
        }
    }
}
