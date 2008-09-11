////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File: �gamePlayState.cpp�
// Author: Scott Smallback (SS) / Jared Hamby (JH)
// Purpose: This is the code file for the game play state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "gamePlayState.h"
#include "..\..\Wrappers\CSGD_DirectInput.h"
#include "..\..\EventSystem\eventManager.h"
#include "..\..\EventSystem\eventIDs.h"
#include "../../EventSystem/gameEvent.h"
#include "../../EventSystem/playHandler.h"
#include "../../Objects/objManager.h"
#include "..\..\Engines\CTileEngine.h"
#include "..\..\Engines\CAIEngine.h"
#include "..\..\game.h"
#include "levelChooseState.h"
#include "../../helpers/datatypes.h"
#include "../../wrappers/mouse.h"
#include "../../Objects/playerObj.h"
#include "../../Engines/CParticleEffectManager.h"
#include "..\..\Helpers\bitFont.h"

gamePlayState::gamePlayState() {}
gamePlayState::~gamePlayState() {}

gamePlayState* gamePlayState::getInstance()
{
	static gamePlayState playa;

	return &playa;
}

void gamePlayState::enter(void)
{
	theMouse = mouse::getInstance();

	OM = objManager::getInstance();
	theInput = CSGD_DirectInput::GetInstance();
	EM = eventManager::getInstance();
	theFont = bitFont::getInstance();

	TE = CTileEngine::GetInstance();
	if(levelChooseState::getInstance()->GetPlayLevel1() == true)
	{
		m_nLevelNum = 1;
		TE->LoadMap("Resource/PS_TestLevel.bmf");
	}
	else if(levelChooseState::getInstance()->GetPlayLevel2() == true)
	{
		m_nLevelNum = 2;
		TE->LoadMap("Resource/PS_TestLevel2.bmf");
	}
	m_nParticleImageID = CParticleEffectManager::GetInstance()->LoadEffect("Resource/PS_SmokeBottomRight.prt");

	m_bTrapActive = false;

	handler = new playHandler;
	handler->initialize();

	for(int c = BEGIN_PLAY_EVENTS; c < END_PLAY_EVENTS; c++)
		EM->registerClient(c, this);

	EM->sendEvent(EVENT_GAMELOADING);

	AIE = new CAIEngine();
}

void gamePlayState::exit(void)
{
	while(rendering)
		Sleep(1);

	gameState::exit();
	m_bTrapActive = false;

	TE->ShutDown();
	delete AIE;
	OM->clear();
	handler->shutdown();
	delete handler;

	theMouse->shutdown();
	CParticleEffectManager::GetInstance()->Unload(m_nParticleImageID);

}

bool gamePlayState::input(float dt)
{

	if (theInput->KeyPressed(DIK_ESCAPE) || theInput->KeyPressed(DIK_F10))
		EM->sendEvent(EVENT_GAMEPAUSE);

	if(entered)
	{
		if(theInput->KeyDown(game::GetInstance()->GetKeys().m_nRunLeft))
		EM->sendEvent(EVENT_PLAYERGOLEFT);
	else if(theInput->KeyDown(game::GetInstance()->GetKeys().m_nRunRight))
		EM->sendEvent(EVENT_PLAYERGORIGHT);
	else if(theInput->KeyReleased(game::GetInstance()->GetKeys().m_nRunLeft)
		|| theInput->KeyReleased(game::GetInstance()->GetKeys().m_nRunRight))
		EM->sendEvent(EVENT_PLAYERSTOP);

	if(theInput->KeyPressed(game::GetInstance()->GetKeys().m_nJump))
		EM->sendEvent(EVENT_PLAYERJUMP);
	}

	if(theInput->MouseButtonPressedEx(0))
		EM->sendEvent(EVENT_PLAYERFIRE);

	if(theInput->MouseButtonPressedEx(1))
		EM->sendEvent(EVENT_PLAYERSPECFIRE);

	if(theInput->MouseButtonReleased(0) || theInput->MouseButtonReleased(1))
		EM->sendEvent(EVENT_PLAYERCEASEFIRE);

	return true;
}

void gamePlayState::update(float dt)
{
	if(entered)
	{
		AIE->update();

		OM->checkCollisions();

		OM->update(dt);

		theMouse->update(dt);
	}

	CParticleEffectManager::GetInstance()->Update(dt);

	EM->processEvents();
}

void gamePlayState::render(void) const
{
	if(!entered)
		return;


	OM->render();

	if(TE)
		TE->Render();

	
	theMouse->render();
	//NEED TO SPRINT_F THIS, WOULDN'T WORK FOR ME.. LOLOLOL?
	if(m_nLevelNum == 1)
		theFont->drawText("Level: 1", 200, 30, 0xffffffff, .5f);
	else if (m_nLevelNum == 2)
		theFont->drawText("Level: 2", 200, 30, 0xffffffff, .5f);
	else if (m_nLevelNum == 3)
		theFont->drawText("Level: 3", 200, 30, 0xffffffff, .5f);

	theFont->drawText("Time: 0.00", 1600, 30, 0xffffffff, .5f);

	if(m_bTrapActive == true)
	{
		CParticleEffectManager::GetInstance()->Render(m_nParticleImageID,TE->GetTraps()[0].x,TE->GetTraps()[0].y);
	}

}

void gamePlayState::HandleEvent(gameEvent* ev)
{
	switch(ev->getEventID())
	{
	case EVENT_LEVELLOADED:
		entered = true;
		break;
	case EVENT_LEVELFINISHED:
		entered = false;
		break;
	}
}