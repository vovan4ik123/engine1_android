#ifndef ENGINE1_GAME_H
#define ENGINE1_GAME_H

#include "SDL.h"
#include "GameStateMachine.h"
#include <memory>
#include "utility/logs.h"

class Game
{
public:
    void init();

    // loop
    void updateEventHandler();
    void update();
    void draw();
    void playSound();

    void endGame();

    void reCreateWindow();

    static Game* instance()
    {
        traceMethod();

        static Game inst;
        return &inst;
    }

    bool isRunning() { traceMethod(); return m_running;}
    int getScreenWidth() { traceMethod(); return m_screenWidth;}
    int getScreenHeight() { traceMethod(); return m_screenHeight;}

    const std::shared_ptr<GameStateMachine> getStateMachine() const { traceMethod(); return m_gameStateMachine;}

private:
    Game();
    ~Game();

    bool m_running = false;
    int m_screenWidth{};
    int m_screenHeight{};

    SDL_DisplayMode m_DM{};
    SDL_Window* m_window{};
    SDL_GLContext m_glContext{};

    std::shared_ptr<GameStateMachine> m_gameStateMachine;
};


#endif //ENGINE1_GAME_H
