#include "GameStateMachine.h"
#include "SDL.h"

GameStateMachine::GameStateMachine()
{
    traceMethod();

    m_gameStates.reserve(5);
    m_gameStatesForDelete.reserve(5);

}

GameStateMachine::~GameStateMachine()
{
    traceMethod();
}

void GameStateMachine::update()
{
    traceMethod();

    if(!m_gameStates.empty()) m_gameStates.back()->update();

    // only after gameState->update() finished work !!!!
    if(!m_gameStatesForDelete.empty()) m_gameStatesForDelete.clear();
}

void GameStateMachine::draw()
{
    traceMethod();

    if(!m_gameStates.empty()) m_gameStates.back()->draw();
}

void GameStateMachine::playSound()
{
    traceMethod();

    if(!m_gameStates.empty()) m_gameStates.back()->playSound();
}

bool GameStateMachine::pushState(std::shared_ptr<GameState> state)
{
    traceMethod();

    SDL_Log("GameStateMachine::pushState()");
    m_gameStates.push_back(state);
    if(m_gameStates.back()->onEnter()) return true;
    else return false;
}

bool GameStateMachine::changeState(std::shared_ptr<GameState> state)
{
    traceMethod();

    SDL_Log("GameStateMachine::changeState()");
    if(m_gameStates.empty())
    {
        if(pushState(state)) return true;
        else return false;
    } else
    {
        if(m_gameStates.back()->getStateID() == state->getStateID()) {return true;}

        if(!popState()) return false;
        if(!pushState(state)) return false;

        return true;
    }
}

bool GameStateMachine::popState()
{
    traceMethod();

    SDL_Log("GameStateMachine::popState()");
    if(!m_gameStates.empty())
    {
        if(m_gameStates.back()->onExit())
        {
            m_gameStatesForDelete.push_back(m_gameStates.back());
            m_gameStates.pop_back();
            return true;
        }
    }
    return false;
}