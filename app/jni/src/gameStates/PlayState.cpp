#include "PlayState.h"
#include "PauseState.h"
#include "SDL.h"
#include "Game.h"
#include "Level_1.h"
#include "UtilityMethods.h"

PlayState::PlayState()
{
    traceMethod();
    SDL_Log("PlayState()");


    m_ID = GameStateID::PLAY_STATE;
}

PlayState::~PlayState()
{
    traceMethod();
    SDL_Log("~PlayState()");
}

bool PlayState::onEnter()
{
    traceMethod();

    // read saved lvl from database and create it and load.

    return setLevel(GameLevelID::LEVEL_1);
}

bool PlayState::onExit()
{
    traceMethod();

    return true;
}

void PlayState::update()
{
    traceMethod();

    m_gameLevel->update();

    if(m_gameLevel->isLevelCompleted())
    {
        int x = int(m_gameLevel->getLevelID());
        setLevel(Utils::intToLevelID(++x)); // load next
    }
    if(m_gameLevel->isLevelFailed()) setLevel(m_gameLevel->getLevelID());
}

void PlayState::draw()
{
    traceMethod();

    m_gameLevel->draw();
}

bool PlayState::setLevel(GameLevelID id)
{
    traceMethod();

    switch(id)
    {
        case GameLevelID::LEVEL_1 :
            // physics world is destroyed in Level_1 destructor
            m_gameLevel = std::make_unique<Level_1>();
            // physics world is created in Level_1 constructor
            break;
        case GameLevelID::LEVEL_2 :

            break;
        case GameLevelID::LEVEL_3 :

            break;
        case GameLevelID::LEVEL_4 :

            break;

    }

    return false;
}

void PlayState::playSound()
{
    traceMethod();

    m_gameLevel->playSound();
}

void PlayState::playToPause()
{
    traceMethod();

    Game::instance()->getStateMachine()->pushState(std::make_shared<PauseState>());
}

void PlayState::playToSettings()
{
    traceMethod();
}

void PlayState::ExitGame()
{
    traceMethod();
}