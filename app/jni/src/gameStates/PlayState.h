#ifndef ENGINE1_PLAYSTATE_H
#define ENGINE1_PLAYSTATE_H

#include "GameState.h"
#include "GameObject.h"
#include "GameLevel.h"
#include <memory>

class PlayState : public GameState
{
public:
    PlayState();
    virtual ~PlayState() override;

    virtual bool onEnter() override;

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

    virtual bool onExit() override;

    static void playToPause();
    static void playToSettings();
    static void ExitGame();

private:
    bool setLevel(GameLevelID id);

    std::unique_ptr<GameLevel> m_gameLevel{};
};


#endif //ENGINE1_PLAYSTATE_H
