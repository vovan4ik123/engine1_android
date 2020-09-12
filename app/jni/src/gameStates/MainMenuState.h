#ifndef ENGINE1_MAINMENUSTATE_H
#define ENGINE1_MAINMENUSTATE_H

#include "GameState.h"
#include "GameObject.h"
#include <vector>

class MainMenuState : public GameState
{
public:
    MainMenuState();
    virtual ~MainMenuState();

    virtual bool onEnter() override;

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

    virtual bool onExit() override;

    static void mainMenuToPlay();
    static void exitGame();

private:
    std::vector<std::shared_ptr<GameObject>> m_gameObjects;
};


#endif //ENGINE1_MAINMENUSTATE_H
