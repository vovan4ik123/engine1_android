#ifndef ENGINE1_GAMESTATE_H
#define ENGINE1_GAMESTATE_H

#include "logs.h"

enum class GameStateID
{
    UNKNOWN_STATE, // return from GameStateMachine if states vector is empty
    MAIN_MENU_STATE,
    PLAY_STATE,
    PAUSE_STATE
};

//Base class for all states in game (menu, pause, play...)
class GameState
{
public:
    virtual ~GameState() {}

    virtual bool onEnter() = 0; // init state here

    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void playSound() = 0;

    virtual bool onExit() = 0; // free state here

    const GameStateID getStateID() const { traceMethod(); return m_ID; };

protected:
    GameStateID m_ID = GameStateID::UNKNOWN_STATE; // initialize in subclass
};

#endif //ENGINE1_GAMESTATE_H
