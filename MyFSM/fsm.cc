#include "fsm.h"

#include <unordered_map>
#include <algorithm>

#include <iostream>

using namespace api;

template<typename K, typename V>
std::unordered_map<V,K> inverse_map(std::unordered_map<K,V> const &map)
{
    std::unordered_map<V,K> inv;
    std::for_each(map.begin(), map.end(),
                [&inv] (const std::pair<K,V> &p)
                {
                    inv.insert(std::make_pair(p.second, p.first));
                });
    return inv;
}

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

#define EVENT(systemEvent) eventArray_[to_underlying(systemEvent)]

namespace fsm
{

bool FSM::s_isSitting = false;
bool FSM::s_isInSession = false;

FSM::FSM()
{
    createEventMapping();
}

void FSM::init()
{
    createSupraStateMachine();
    createMainStateMachine();
    createPauseStateMachine();
    createPauseRecoverStateMachine();
    createStandingUpStateMachine();
    createSittingDownStateMachine();
    createRepositioningStateMachine();
    createWalkingStateMachine();
    createTurningStateMachine();

    // Initialize shared accessor with current init state
    supraState->updateSharedMem();

    auto mapEvent = [this](api::Event event) {
        handleEvent(event);
    };
}

void FSM::createEventMapping()
{
    std::unordered_map<api::Event, std::string> event2name = inverse_map(api::KEY_MAPPING);
    for (std::pair<api::Event, std::string> event : event2name)
    {
        CREATE_EVENT_MAPPING(eventArray_[to_underlying(event.first)] , event.first, event.second);
    }
}

void FSM::createSupraStateMachine()
{
    SET_INIT_STATE(supraState, Startup);

    ADD_TRANSITION (Startup,            EVENT(Event::FINISHED),         StartupClinic);
    ADD_TRANSITION (StartupClinic,      EVENT(Event::FINISHED),         PauseState);
    ADD_TRANSITION (StartupClinic,      EVENT(Event::BUTTON_TRANSFER),      MainState);
    ADD_TRANSITION (StartupClinic,      EVENT(Event::ALERT_ERROR),          GentleTrap);
    ADD_TRANSITION (StartupClinic,      EVENT(Event::UNRECOVERABLE_ERROR),  Trap);
    ADD_TRANSITION (StartupClinic,      EVENT(Event::EMERGENCY_ERROR),      Emergency);

    ADD_TRANSITION (MainState,          EVENT(Event::ALERT_ERROR),          GentleTrap);
    ADD_TRANSITION (MainState,          EVENT(Event::UNRECOVERABLE_ERROR),  Trap);
    ADD_TRANSITION (MainState,          EVENT(Event::EMERGENCY_ERROR),      Emergency);
    ADD_TRANSITION (MainState,          EVENT(Event::BUTTON_PAUSE),         PauseRecoverState);
    ADD_TRANSITION (MainState,          EVENT(Event::PAUSE),                PauseState);
    ADD_TRANSITION (MainState,          EVENT(Event::TIME_OUT),             PauseState);
    
    ADD_TRANSITION (PauseRecoverState,  EVENT(Event::FINISHED),         MainState);
    ADD_TRANSITION (PauseRecoverState,  EVENT(Event::ALERT_ERROR),          GentleTrap);
    ADD_TRANSITION (PauseRecoverState,  EVENT(Event::UNRECOVERABLE_ERROR),  Trap);
    ADD_TRANSITION (PauseRecoverState,  EVENT(Event::EMERGENCY_ERROR),      Emergency);
    
    ADD_TRANSITION (PauseState,         EVENT(Event::BUTTON_PAUSE),         PauseRecoverState);
    ADD_TRANSITION (PauseState,         EVENT(Event::FINISHED),         MainState);
    ADD_TRANSITION (PauseState,         EVENT(Event::ALERT_ERROR),          GentleTrap);
    ADD_TRANSITION (PauseState,         EVENT(Event::UNRECOVERABLE_ERROR),  Trap);
    ADD_TRANSITION (PauseState,         EVENT(Event::EMERGENCY_ERROR),      Emergency);

    ADD_TRANSITION (GentleTrap,         EVENT(Event::ALERT_ERROR),          Trap);
    ADD_TRANSITION (GentleTrap,         EVENT(Event::UNRECOVERABLE_ERROR),  Trap);
    ADD_TRANSITION (GentleTrap,         EVENT(Event::EMERGENCY_ERROR),      Emergency);

    ADD_TRANSITION (Trap,               EVENT(Event::UNRECOVERABLE_ERROR),  Emergency);
    ADD_TRANSITION (Trap,               EVENT(Event::EMERGENCY_ERROR),      Emergency);

    ADD_TRANSITION (PauseState,         EVENT(Event::FINISHED),         MainState);
}

void FSM::createMainStateMachine()
{
    SET_INIT_STATE(MainState, Standing);

    SET_ON_ENTRY(MainState, [this]{
        if (s_isInSession && s_isSitting)
        {
            SET_INIT_STATE(MainState, Sitting);
        }
        else if (s_isInSession && not s_isSitting)
        {
            SET_INIT_STATE(MainState, Standing);
        }
        else if (not s_isInSession && s_isSitting)
        {
            SET_INIT_STATE(MainState, Installation);
        }
        else
        {
            SET_INIT_STATE(MainState, GentleTrap);
        }
    });

    ADD_TRANSITION (Sitting,            EVENT(Event::BUTTON_STANDSTILL),    StandingUpState);
    ADD_TRANSITION (Sitting,            EVENT(Event::BUTTON_TRANSFER),      Installation);
    ADD_TRANSITION (Installation,       EVENT(Event::FINISHED),         PostInstallation);
    ADD_TRANSITION (PostInstallation,   EVENT(Event::FINISHED),         Sitting);

    ADD_TRANSITION (Standing,           EVENT(Event::BUTTON_STAND_TO_SIT),  SittingDownState);
    ADD_TRANSITION (Standing,           EVENT(Event::BUTTON_STANDSTILL),    Exercising);
    ADD_TRANSITION (Standing,           EVENT(Event::BUTTON_TURN_AROUND),   TurningState);
    ADD_TRANSITION (Standing,           EVENT(Event::BUTTON_WALK),          WalkingState);
    ADD_TRANSITION (Standing,           EVENT(Event::BUTTON_REPOSITIONING), RepositioningState);

    ADD_TRANSITION (StandingUpState,    EVENT(Event::FINISHED),         Standing);
    ADD_TRANSITION (StandingUpState,    EVENT(Event::TIME_OUT),             Sitting);

    ADD_TRANSITION (SittingDownState,   EVENT(Event::FINISHED),         Sitting);
    ADD_TRANSITION (SittingDownState,   EVENT(Event::TIME_OUT),             Standing);

    ADD_TRANSITION (Exercising,         EVENT(Event::FINISHED),         Standing);
    ADD_TRANSITION (Exercising,         EVENT(Event::FAILURE),          PauseState);
    ADD_TRANSITION (Exercising,         EVENT(Event::TIME_OUT),             RecoverStanding);

    ADD_TRANSITION (TurningState,       EVENT(Event::FINISHED),         Standing);
    ADD_TRANSITION (TurningState,       EVENT(Event::TIME_OUT),             Standing);

    ADD_TRANSITION (WalkingState,       EVENT(Event::FINISHED),         RecoverStanding);
    ADD_TRANSITION (WalkingState,       EVENT(Event::BUTTON_STANDSTILL),    Standing);
    ADD_TRANSITION (WalkingState,       EVENT(Event::TIME_OUT),             Standing);
    ADD_TRANSITION (WalkingState,       EVENT(Event::FAILURE),          RecoverStanding);
    ADD_TRANSITION (WalkingState,       EVENT(Event::WALK_FAILURE),         RecoverStanding);

    ADD_TRANSITION (RepositioningState, EVENT(Event::BUTTON_STANDSTILL),    Standing);
    ADD_TRANSITION (RepositioningState, EVENT(Event::FINISHED),         Standing);
    ADD_TRANSITION (RepositioningState, EVENT(Event::TIME_OUT),             Standing);

    ADD_TRANSITION (RecoverStanding,    EVENT(Event::FINISHED),         Standing);
    ADD_TRANSITION (RecoverStanding,    EVENT(Event::FINISHED),         Sitting);


}

void FSM::createStandingUpStateMachine()
{
    SET_INIT_STATE(StandingUpState, PrepareToStand);
    SET_ON_ENTRY(StandingUpState, [this]{SET_INIT_STATE(StandingUpState, PrepareToStand);});

    ADD_TRANSITION (PrepareToStand, EVENT(Event::FINISHED),     TriggerStandUp);
    ADD_TRANSITION (TriggerStandUp, EVENT(Event::FINISHED),     StandingUp);
    ADD_TRANSITION (TriggerStandUp, EVENT(Event::TIME_OUT),         Relax);
    ADD_TRANSITION (TriggerStandUp, EVENT(Event::BUTTON_SITTING),   Relax);
    ADD_TRANSITION (StandingUp,     EVENT(Event::BUTTON_PAUSE),     PauseState);
    ADD_TRANSITION (StandingUp,     EVENT(Event::PAUSE_SITTING),    RecoverSitting);
}

void FSM::createSittingDownStateMachine()
{
    SET_INIT_STATE(SittingDownState, PrepareToSit);
    SET_ON_ENTRY(SittingDownState, [this]{SET_INIT_STATE(SittingDownState, PrepareToSit);});

    ADD_TRANSITION (PrepareToSit,   EVENT(Event::FINISHED),         TriggerSitDown);
    ADD_TRANSITION (TriggerSitDown, EVENT(Event::FINISHED),         SittingDown);
    ADD_TRANSITION (TriggerSitDown, EVENT(Event::BUTTON_STANDSTILL),    RecoverStanding);
    ADD_TRANSITION (SittingDown,    EVENT(Event::FINISHED),         Relax);
    ADD_TRANSITION (SittingDown,    EVENT(Event::BUTTON_PAUSE),         Pause);
}

void FSM::createPauseStateMachine()
{
    SET_INIT_STATE(PauseState, Pause);
    SET_ON_ENTRY(PauseState, [this]{SET_INIT_STATE(PauseState, Pause);});

    ADD_TRANSITION (Pause,          EVENT(Event::PAUSE_SITTING),        PauseToSitting);
    ADD_TRANSITION (Pause,          EVENT(Event::PAUSE_STANDING),       PauseToStanding);
    ADD_TRANSITION (PauseToSitting, EVENT(Event::BUTTON_SITTING),       RecoverSitting);
    ADD_TRANSITION (PauseToStanding,EVENT(Event::BUTTON_STANDSTILL),    RecoverStanding);
}

void FSM::createPauseRecoverStateMachine()
{
    SET_INIT_STATE(PauseRecoverState, PauseToRecover);
    SET_ON_ENTRY(PauseRecoverState, [this]{SET_INIT_STATE(PauseRecoverState, PauseToRecover);});

    ADD_TRANSITION (PauseToRecover,  EVENT(Event::PAUSE_SITTING),   RecoverSitting);
    ADD_TRANSITION (PauseToRecover,  EVENT(Event::PAUSE_STANDING),  RecoverStanding);
}

void FSM::createRepositioningStateMachine()
{
    SET_INIT_STATE(RepositioningState, RepositionTrigger);
    SET_ON_ENTRY(RepositioningState, [this]{SET_INIT_STATE(RepositioningState, RepositionTrigger);});

    ADD_TRANSITION (RepositionTrigger,   EVENT(Event::REPOSITIONING_BACK),  RepositioningBack);
    ADD_TRANSITION (RepositionTrigger,   EVENT(Event::REPOSITIONING_RIGHT), RepositioningRight);
    ADD_TRANSITION (RepositionTrigger,   EVENT(Event::REPOSITIONING_LEFT),  RepositioningLeft);

}

void FSM::createWalkingStateMachine()
{
    SET_INIT_STATE(WalkingState, WalkInit);
    SET_ON_ENTRY(WalkingState, [this]{SET_INIT_STATE(WalkingState, WalkInit);});

    ADD_TRANSITION (WalkInit,       EVENT(Event::FINISHED),         WalkTrigger);
    ADD_TRANSITION (WalkTrigger,    EVENT(Event::FINISHED),         Walking);
    ADD_TRANSITION (WalkTrigger,    EVENT(Event::BUTTON_WALK_ASSIST),   AssistWalking);

    ADD_TRANSITION (AssistWalking,  EVENT(Event::FINISHED),         RecoverStanding);
}

void FSM::createTurningStateMachine()
{
    SET_INIT_STATE(TurningState, TurnInit);
    SET_ON_ENTRY(TurningState, [this]{SET_INIT_STATE(TurningState, TurnInit);});

    ADD_TRANSITION (TurnInit,       EVENT(Event::FINISHED),         TurnTrigger);
    ADD_TRANSITION (TurnTrigger,    EVENT(Event::TURNAROUND_LEFT),      TurningLeft);
    ADD_TRANSITION (TurnTrigger,    EVENT(Event::TURNAROUND_RIGHT),     TurningRight);
}


void FSM::handleEvent(api::Event eventIn)
{
    supraState->handleEvent(EVENT(eventIn));
}

EventId& FSM::getEvent(api::Event event)
{
    return EVENT(event);
}
}
