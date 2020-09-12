#include "Game.h"
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "SDL_mixer.h"
#include "EventHandler.h"
#include "MainMenuState.h"
#include "UtilityMethods.h"

Game::Game()
{
    traceMethod();
}

Game::~Game()
{
    traceMethod();
}

void Game::init()
{
    traceMethod();

    SDL_Log("Game init");
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) Utils::error("SDL init error in Game.cpp");

    // SET ATTRIBUTE ONLY after initialize
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // set to 1 to require hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // SDL antialiasing for real device. Dont work on emulator !!
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1); // enable antialiasing sdl
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4); // 0 4 8

    SDL_GetCurrentDisplayMode(0, &m_DM);
    m_screenWidth = m_DM.w;
    m_screenHeight = m_DM.h;

    SDL_Log("screenWidth = %i, screenHeight = %i", m_screenWidth, m_screenHeight);

    // Create an application window with the following settings:
    m_window = SDL_CreateWindow("Game", 0, 0, m_screenWidth, m_screenHeight,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if(NULL == m_window)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
        m_window = SDL_CreateWindow("Game", 0, 0, m_screenWidth, m_screenHeight,
                SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    }

    if (NULL == m_window) Utils::error("SDL window create error in Game.cpp");

    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);

    m_glContext = SDL_GL_CreateContext(m_window);

    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // background color after clear screen

    // MIX_DEFAULT_FREQUENCY = sample rate = frequensy = speed playing				// size (speed playing )
    if (-1 == Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
        Utils::error("SDL mixer init error in Game.cpp");

    int ff;
    glGetIntegerv(GL_BLEND, &ff); // if enabled ff = 1, in not ff = 0
    SDL_Log("GL_BLEND = %i", ff);
    glGetIntegerv(GL_DEPTH_TEST, &ff);
    SDL_Log("GL_DEPTH_TEST = %i", ff);
    const GLubyte *version = glGetString(GL_VERSION);
    SDL_Log("OpenGL version on phone = %s", version);

    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &ff);
    SDL_Log("SDL_GL_DEPTH_SIZE = %d", ff);

    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &ff);
    SDL_Log("SDL_GL_MULTISAMPLEBUFFERS = %d", ff);

    SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &ff);
    SDL_Log("SDL_GL_MULTISAMPLESAMPLES = %d", ff);

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &ff);
    SDL_Log("SDL_GL_CONTEXT_MAJOR_VERSION = %d", ff);

    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &ff);
    SDL_Log("SDL_GL_CONTEXT_MINOR_VERSION = %d", ff);

    m_gameStateMachine = std::make_shared<GameStateMachine>();
    if(m_gameStateMachine->pushState(std::make_shared<MainMenuState>()))
    {
        m_running = true;
    }
}

void Game::reCreateWindow()
{
    traceMethod();

    SDL_DestroyWindow(m_window);

    SDL_GetCurrentDisplayMode(0, &m_DM);
    m_screenWidth = m_DM.w;
    m_screenHeight = m_DM.h;
    SDL_Log("screenWidth = %i, screenHeight = %i", m_screenWidth, m_screenHeight);

    m_window = SDL_CreateWindow("Game", 0, 0, m_screenWidth, m_screenHeight,
                                SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);

    SDL_GL_MakeCurrent(m_window, m_glContext); // set old context for new window

    glViewport(0, 0, m_screenWidth, m_screenHeight); // for current resolution
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::updateEventHandler()
{
    traceMethod();

    EventHandler::instance()->updateEventHandler();
}

void Game::update()
{
    traceMethod();

    m_gameStateMachine->update();

    // reset all touch states after update and prepare them for next screen touch events.
    EventHandler::instance()->resetFingerEvents();
}

void Game::draw()
{
    traceMethod();

    //clear all possible buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //	| GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);

    m_gameStateMachine->draw();

    // draw
    //glFinish(); // waiting for finish drawing
    glFlush(); // dont waiting for finish drawing
    SDL_GL_SwapWindow(m_window);
}

void Game::playSound()
{
    traceMethod();

    m_gameStateMachine->playSound();
}

void Game::endGame()
{
    traceMethod();

    m_running = false;

    SDL_GL_DeleteContext(m_glContext);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}