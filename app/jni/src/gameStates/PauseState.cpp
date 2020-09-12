#include "PauseState.h"
#include "Game.h"
#include "EventHandler.h"
#include "Button.h"
#include "MainMenuState.h"

PauseState::PauseState()
{
    traceMethod();
    SDL_Log("PauseState()");

    m_ID = GameStateID::PAUSE_STATE;
}

PauseState::~PauseState()
{
    traceMethod();
    SDL_Log("~PauseState()");
}

bool PauseState::onEnter()
{
    traceMethod();

    std::shared_ptr<Button> mainMenuButton = std::make_shared<Button>(700.0f, 200.0f, 1.0f, 300.0f, 300.0f, "images/main_menu.png");
    mainMenuButton->setButtonAction(&PauseState::pauseToMainMenu);
    m_gameObjects.push_back(mainMenuButton);

    std::shared_ptr<Button> resumeButton = std::make_shared<Button>(500.0f, 700.0f, 1.0f, 240.0f, 100.0f, "images/resume.png");
    resumeButton->setButtonAction(&PauseState::pauseToPlay);
    m_gameObjects.push_back(resumeButton);

    return true;
}

bool PauseState::onExit()
{
    traceMethod();

    return true;
}

void PauseState::update()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->update();
    }
}

void PauseState::draw()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->draw();
    }
}

void PauseState::playSound()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->playSound();
    }
}

void PauseState::pauseToPlay() // come back back to play state
{
    traceMethod();

    Game::instance()->getStateMachine()->popState();
}

void PauseState::pauseToMainMenu() // delete states and push new
{
    traceMethod();

    Game::instance()->getStateMachine()->popState();
    Game::instance()->getStateMachine()->changeState(std::make_shared<MainMenuState>());
}

void PauseState::saveGame()
{
    traceMethod();
}