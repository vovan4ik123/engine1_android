#ifndef ENGINE1_GAMESTATEMACHINE_H
#define ENGINE1_GAMESTATEMACHINE_H

#include "GameState.h"
#include <vector>
#include <memory>
#include "utility/logs.h"

class GameStateMachine
{
public:
    GameStateMachine();
    ~GameStateMachine();

    void update();
    void draw();
    void playSound();

    bool pushState(std::shared_ptr<GameState> state);
    bool changeState(std::shared_ptr<GameState> state);
    bool popState();

    int getStatesCount() { traceMethod(); return m_gameStates.size();}

    const GameStateID getStateID()
    {
        traceMethod();

        if(!m_gameStates.empty()) return m_gameStates.back()->getStateID();
        else return GameStateID::UNKNOWN_STATE;
    }

private:
    std::vector<std::shared_ptr<GameState>> m_gameStates;
    std::vector<std::shared_ptr<GameState>> m_gameStatesForDelete;
};


#endif //ENGINE1_GAMESTATEMACHINE_H
