// GTest
#include <gtest/gtest.h>

// Tested class
#include "../fsm.h"

using namespace fsm;
using namespace api;
using namespace core;

class FSM_Tests : public testing::Test
{
protected:

    FSM_Tests()
        : testing::Test()
    {
    }

    void SetUp()
    {
        fsm_.init();

        supraState_ = fsm_.getFSM();
    }

    void TearDown()
    {}

    void Tick(Event const& event)
    {
        fsm_.getFSM()->run();
        //fsm_.getFSM()->handleEvent(fsm_.getEvent(event));
        fsm_.handleEvent(event);
    }

    bool isState(std::string const& str)
    {
        // testing class member
        bool result = supraState_->isState(str);
        return result;
    }

    void StartClinicInSessionSitting();
    void StartClinicInSessionStanding();

    void StartClinicOutSessionSitting();
    void StartClinicOutSessionStanding();

    FSM fsm_;
    StateManager supraState_;

};

void FSM_Tests::StartClinicOutSessionSitting()
{
    FSM::s_isSitting = true;
    FSM::s_isInSession = false;

    ASSERT_TRUE(isState("Startup"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("StartupClinic"));
    Tick(Event::BUTTON_TRANSFER);
    ASSERT_TRUE(isState("Installation"));

}

void FSM_Tests::StartClinicOutSessionStanding()
{
    FSM::s_isSitting = false;
    FSM::s_isInSession = false;

    ASSERT_TRUE(isState("Startup"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("StartupClinic"));
    Tick(Event::BUTTON_TRANSFER);
    ASSERT_TRUE(isState("GentleTrap"));

}

void FSM_Tests::StartClinicInSessionSitting()
{
    FSM::s_isSitting = true;
    FSM::s_isInSession = true;

    ASSERT_TRUE(isState("Startup"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("StartupClinic"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Pause"));
    Tick(Event::PAUSE_SITTING);
    ASSERT_TRUE(isState("PauseToSitting"));
    Tick(Event::BUTTON_SITTING);
    ASSERT_TRUE(isState("RecoverSitting"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Sitting"));
}

void FSM_Tests::StartClinicInSessionStanding()
{
    FSM::s_isSitting = false;
    FSM::s_isInSession = true;

    ASSERT_TRUE(isState("Startup"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("StartupClinic"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Pause"));
    Tick(Event::PAUSE_STANDING);
    ASSERT_TRUE(isState("PauseToStanding"));
    Tick(Event::BUTTON_STANDSTILL);
    ASSERT_TRUE(isState("RecoverStanding"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));
}

TEST_F(FSM_Tests, CompleteFSM_OutSession_Sitting)
{
    StartClinicOutSessionSitting();
    ASSERT_TRUE(isState("Installation"));
}

TEST_F(FSM_Tests, CompleteFSM_OutSession_Standing)
{
    StartClinicOutSessionStanding();
    ASSERT_TRUE(isState("GentleTrap"));
}

TEST_F(FSM_Tests, CompleteFSM_InSession_Sitting)
{
    StartClinicInSessionSitting();
    ASSERT_TRUE(isState("Sitting"));

    Tick(Event::BUTTON_TRANSFER);
    ASSERT_TRUE(isState("Installation"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("PostInstallation"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Sitting"));

    Tick(Event::BUTTON_STANDSTILL);
    ASSERT_TRUE(isState("PrepareToStand"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("TriggerStandUp"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("StandingUp"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));
}

TEST_F(FSM_Tests, CompleteFSM_InSession_Standing)
{
    StartClinicInSessionStanding();
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_STANDSTILL);
    ASSERT_TRUE(isState("Exercising"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_WALK);
    ASSERT_TRUE(isState("WalkInit"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("WalkTrigger"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Walking"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("RecoverStanding"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_TURN_AROUND);
    ASSERT_TRUE(isState("TurnInit"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("TurnTrigger"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_REPOSITIONING);
    ASSERT_TRUE(isState("RepositionTrigger"));
    Tick(Event::BUTTON_STANDSTILL);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_STAND_TO_SIT);
    ASSERT_TRUE(isState("PrepareToSit"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("TriggerSitDown"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("SittingDown"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Relax"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Sitting"));
}

TEST_F(FSM_Tests, CompleteFSM_Pause)
{
    StartClinicInSessionStanding();
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_STANDSTILL);
    ASSERT_TRUE(isState("Exercising"));

    Tick(Event::BUTTON_PAUSE);
    ASSERT_TRUE(isState("PauseToRecover"));

    Tick(Event::PAUSE_STANDING);
    ASSERT_TRUE(isState("RecoverStanding"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));
}

TEST_F(FSM_Tests, CompleteFSM_Errors)
{
    StartClinicInSessionStanding();
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_STANDSTILL);
    ASSERT_TRUE(isState("Exercising"));

    Tick(Event::ALERT_ERROR);
    ASSERT_TRUE(isState("GentleTrap"));
    Tick(Event::ALERT_ERROR);
    ASSERT_TRUE(isState("Trap"));
    Tick(Event::UNRECOVERABLE_ERROR);
    ASSERT_TRUE(isState("Emergency"));
}

TEST_F(FSM_Tests, CompleteFSM_Repositioning)
{
    StartClinicInSessionStanding();
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_REPOSITIONING);
    ASSERT_TRUE(isState("RepositionTrigger"));

    Tick(Event::REPOSITIONING_BACK);
    ASSERT_TRUE(isState("RepositioningBack"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_REPOSITIONING);
    ASSERT_TRUE(isState("RepositionTrigger"));    
    
    Tick(Event::REPOSITIONING_RIGHT);
    ASSERT_TRUE(isState("RepositioningRight"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_REPOSITIONING);
    ASSERT_TRUE(isState("RepositionTrigger"));    
    
    Tick(Event::REPOSITIONING_LEFT);
    ASSERT_TRUE(isState("RepositioningLeft"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

}

TEST_F(FSM_Tests, CompleteFSM_Walking)
{
    StartClinicInSessionStanding();
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_WALK);
    ASSERT_TRUE(isState("WalkInit"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("WalkTrigger"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Walking"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("RecoverStanding"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_WALK);
    ASSERT_TRUE(isState("WalkInit"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("WalkTrigger"));

    Tick(Event::BUTTON_WALK_ASSIST);
    ASSERT_TRUE(isState("AssistWalking"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("RecoverStanding"));

    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("Standing"));
}

TEST_F(FSM_Tests, CompleteFSM_Turning)
{
    StartClinicInSessionStanding();
    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_TURN_AROUND);
    ASSERT_TRUE(isState("TurnInit"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("TurnTrigger"));
    Tick(Event::TURNAROUND_LEFT);
    ASSERT_TRUE(isState("TurningLeft"));
    Tick(Event::FINISHED);

    ASSERT_TRUE(isState("Standing"));

    Tick(Event::BUTTON_TURN_AROUND);
    ASSERT_TRUE(isState("TurnInit"));
    Tick(Event::FINISHED);
    ASSERT_TRUE(isState("TurnTrigger"));
    Tick(Event::TURNAROUND_RIGHT);
    ASSERT_TRUE(isState("TurningRight"));
    Tick(Event::FINISHED);

    ASSERT_TRUE(isState("Standing"));
}



