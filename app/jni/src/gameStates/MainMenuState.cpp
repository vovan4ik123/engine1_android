#include "MainMenuState.h"
#include "Game.h"
#include "EventHandler.h"
#include "Button.h"
#include "PlayState.h"
#include "Text.h"

MainMenuState::MainMenuState()
{
    traceMethod();
    SDL_Log("MainMenuState()");

    m_ID = GameStateID::MAIN_MENU_STATE;
}

MainMenuState::~MainMenuState()
{
    traceMethod();
    SDL_Log("~MainMenuState()");
}

bool MainMenuState::onEnter()
{
    traceMethod();

    std::shared_ptr<Button> playButton = std::make_shared<Button>(500.0f, 300.0f, 1.0f, 500.0f, 300.0f, "images/play.png");
    playButton->setButtonAction(&MainMenuState::mainMenuToPlay);
    m_gameObjects.push_back(playButton);

    return true;
}

bool MainMenuState::onExit()
{
    traceMethod();

    return true;
}

void MainMenuState::update()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->update();
    }
}

void MainMenuState::draw()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->draw();
    }
}

void MainMenuState::playSound()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->playSound();
    }
}

void MainMenuState::mainMenuToPlay()
{
    traceMethod();

    Game::instance()->getStateMachine()->changeState(std::make_shared<PlayState>());
}

void MainMenuState::exitGame()
{
    traceMethod();
}