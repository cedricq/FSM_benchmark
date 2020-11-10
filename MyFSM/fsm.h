#ifndef FSM_H_
#define FSM_H_

#include "myfsm.hpp"

#include "Events.h"

#include <array>

using namespace core;

namespace fsm
{

class FSM
{
public:
    FSM();
    ~FSM() {

    }

    void init();

    void handleEvent(api::Event eventIn);

    StateManager getFSM() {return supraState;}

    EventId& getEvent(api::Event event);
    static bool s_isSitting;
    static bool s_isInSession;

private:

    StateManager supraState = std::make_shared<StateManagerClass_>("Supra");
    State Startup           = std::make_shared<State_>("Startup");
    State GentleTrap        = std::make_shared<State_>("GentleTrap");
    State Trap              = std::make_shared<State_>("Trap");
    State Emergency         = std::make_shared<State_>("Emergency");

    StateManager MainState  = std::make_shared<StateManagerClass_>("Main");
    State StartupClinic     = std::make_shared<State_>("StartupClinic");
    State Installation      = std::make_shared<State_>("Installation");
    State PostInstallation  = std::make_shared<State_>("PostInstallation");
    State Sitting           = std::make_shared<State_>("Sitting");
    State Standing          = std::make_shared<State_>("Standing");
    State Exercising        = std::make_shared<State_>("Exercising");
    State Turning           = std::make_shared<State_>("Turning");

    StateManager PauseState  = std::make_shared<StateManagerClass_>("PauseState");
    StateManager PauseRecoverState = std::make_shared<StateManagerClass_>("PauseRecoverState");
    State PauseToRecover    = std::make_shared<State_>("PauseToRecover");
    State Pause             = std::make_shared<State_>("Pause");
    State PauseToSitting    = std::make_shared<State_>("PauseToSitting");
    State PauseToStanding   = std::make_shared<State_>("PauseToStanding");
    State RecoverSitting    = std::make_shared<State_>("RecoverSitting");
    State RecoverStanding   = std::make_shared<State_>("RecoverStanding");

    StateManager StandingUpState = std::make_shared<StateManagerClass_>("StandingUp");
    StateManager SittingDownState = std::make_shared<StateManagerClass_>("SittingDown");
    State PrepareToStand   = std::make_shared<State_>("PrepareToStand");
    State TriggerStandUp   = std::make_shared<State_>("TriggerStandUp");
    State StandingUp       = std::make_shared<State_>("StandingUp");
    State PrepareToSit     = std::make_shared<State_>("PrepareToSit");
    State TriggerSitDown   = std::make_shared<State_>("TriggerSitDown");
    State SittingDown      = std::make_shared<State_>("SittingDown");
    State Relax            = std::make_shared<State_>("Relax");

    StateManager RepositioningState = std::make_shared<StateManagerClass_>("Repositioning");
    State RepositionTrigger   = std::make_shared<State_>("RepositionTrigger");
    State RepositioningBack   = std::make_shared<State_>("RepositioningBack");
    State RepositioningLeft   = std::make_shared<State_>("RepositioningLeft");
    State RepositioningRight  = std::make_shared<State_>("RepositioningRight");

    StateManager WalkingState = std::make_shared<StateManagerClass_>("Walking");
    State WalkInit      = std::make_shared<State_>("WalkInit");
    State WalkTrigger   = std::make_shared<State_>("WalkTrigger");
    State Walking       = std::make_shared<State_>("Walking");
    State AssistWalking = std::make_shared<State_>("AssistWalking");

    StateManager TurningState = std::make_shared<StateManagerClass_>("Turning");
    State TurnInit      = std::make_shared<State_>("TurnInit");
    State TurnTrigger   = std::make_shared<State_>("TurnTrigger");
    State TurningLeft   = std::make_shared<State_>("TurningLeft");
    State TurningRight  = std::make_shared<State_>("TurningRight");

    std::array<EventId, static_cast<int32_t>(api::Event::N_EVENTS)> eventArray_ {};

    void createEventMapping();

    void createSupraStateMachine();
    void createMainStateMachine();
    void createPauseStateMachine();
    void createPauseRecoverStateMachine();
    void createStandingUpStateMachine();
    void createSittingDownStateMachine();
    void createRepositioningStateMachine();
    void createWalkingStateMachine();
    void createTurningStateMachine();

};

}

#endif
