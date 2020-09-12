#ifndef ENGINE1_GAMELEVEL_H
#define ENGINE1_GAMELEVEL_H

#include "logs.h"


enum class GameLevelID
{
    UNKNOWN_LEVEL,
    LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_5,
    LEVEL_6, LEVEL_7, LEVEL_8, LEVEL_9, LEVEL_10,

    COUNT   // count indicates one beyond the last element
            // (--(int(GameLevelID::COUNT))) = last element
};

// Base class for all Levels
class GameLevel
{
public:
    virtual ~GameLevel() {}

    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void playSound() = 0;

    const GameLevelID getLevelID() const { traceMethod(); return m_ID; };

    const bool isLevelCompleted() const { traceMethod(); return m_levelCompleted; }
    const bool isLevelFailed() const { traceMethod(); return m_levelFailed; }

protected:
    bool m_levelCompleted = false; // load next
    bool m_levelFailed = false; // load this again
    GameLevelID m_ID = GameLevelID::UNKNOWN_LEVEL;
};

#endif //ENGINE1_GAMELEVEL_H
