#ifndef ENGINE1_PAUSESTATE_H
#define ENGINE1_PAUSESTATE_H

#include "GameState.h"
#include "GameObject.h"
#include <vector>

class PauseState : public GameState
{
public:
    PauseState();
    virtual ~PauseState();

    virtual bool onEnter() override;

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

    virtual bool onExit() override;

    static void pauseToPlay();
    static void pauseToMainMenu();
    static void saveGame();

private:
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
};


#endif //ENGINE1_PAUSESTATE_H
