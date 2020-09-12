#include "EventHandler.h"
#include "Game.h"
#include "PauseState.h"
#include <memory>

EventHandler::EventHandler()
{
    traceMethod();

    m_fingerEventState.reserve(8);

    m_fingerEventState.push_back(false);
    m_fingerEventState.push_back(false);
    m_fingerEventState.push_back(false);
    m_fingerEventState.push_back(false);

    m_currentOrientation = SDL_GetDisplayOrientation(0);
}

EventHandler::~EventHandler()
{
    traceMethod();
}

void EventHandler::updateEventHandler()
{
    traceMethod();

    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
//MOBILE APP LIFECYCLE
                // Active(100% visible) -> onPause()-> Visible(partially hidden) -> onStop() -> Background(100% hidden)
                // Active(100% visible) <- onResume() <- Visible(partially hidden) <- onStart() <- Background(100% hidden)

                //The application is low on memory, free memory if possible.
                // Called on iOS in applicationDidReceiveMemoryWarning()
                //Called on Android in onLowMemory()
                case SDL_APP_LOWMEMORY:

                    SDL_Log("EventHandler switch(event.type) : SDL_APP_LOWMEMORY");
                    break;

                    //Prepare for go to background. Can terminated without event.
                    // Prepare for potential terminating here !!!!
                    // Called on iOS in applicationWillResignActive()
                    // Called on Android in onPause()
                case SDL_APP_WILLENTERBACKGROUND:
                    // you can save game here
                    if(Game::instance()->getStateMachine()->getStateID() == GameStateID::PLAY_STATE)
                    {
                        Game::instance()->getStateMachine()->pushState(std::make_shared<PauseState>());
                    }
                    SDL_Log("EventHandler switch(event.type) : SDL_APP_WILLENTERBACKGROUND");
                    break;

                    //already in background
                    //Called on iOS in applicationDidEnterBackground()
                    //Called on Android in onPause()
                case SDL_APP_DIDENTERBACKGROUND:

                    SDL_Log("EventHandler switch(event.type) : SDL_APP_DIDENTERBACKGROUND");
                    break;

                    //prepare to appear in foreground
                    //Called on iOS in applicationWillEnterForeground()
                    //Called on Android in onResume()
                case SDL_APP_WILLENTERFOREGROUND:

                    SDL_Log("EventHandler switch(event.type) : SDL_APP_WILLENTERFOREGROUND");
                    break;

                    //already in foreground
                    //Called on iOS in applicationDidBecomeActive()
                    //Called on Android in onResume()
                case SDL_APP_DIDENTERFOREGROUND:
                    SDL_Log("EventHandler switch(event.type) : SDL_APP_DIDENTERFOREGROUND");

                    break;
// ENDMOBILE APP LIFECYCLE

                    //Android OS can decide to terminate your application by calling onDestroy()
                    //Your application will receive a SDL_QUIT event
                    //Save game before Android will close it
                case SDL_QUIT:
                    SDL_Log("EventHandler switch(event.type) : SDL_QUIT");
                    Game::instance()->endGame();
                    break;

//KEYS
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
                    {
                        SDL_Log("EventHandler Application control keypad : SDL_SCANCODE_AC_BACK");
                    }

                    SDL_Log("EventHandler switch(event.type) : SDL_KEYDOWN");
                    break;

                case SDL_KEYUP:
                    if (event.key.keysym.scancode == SDL_SCANCODE_AC_BACK)
                    {
                        SDL_Log("EventHandler Application control keypad : SDL_SCANCODE_AC_BACK");
                    }

                    SDL_Log("EventHandler switch(event.type) : SDL_KEYUP");
                    break;
//END KEYS

//TOUCH EVENT
                case SDL_FINGERDOWN:
                    m_fingerEventState[FINGER_DOWN] = true;
                    m_fingerEventState[FINGER_UP] = false;

                    m_timeFingerDown = SDL_GetTicks();

                    m_fingerDownPos.x = event.tfinger.x * Game::instance()->getScreenWidth();
                    m_fingerDownPos.y = Game::instance()->getScreenHeight() - (event.tfinger.y * Game::instance()->getScreenHeight());

                    break;

                case SDL_FINGERUP:
                    m_fingerEventState[FINGER_UP] = true;
                    m_fingerEventState[FINGER_DOWN] = false;
                    m_fingerEventState[FINGER_MOTION] = false;

                    if (SDL_GetTicks() < m_timeFingerDown + m_timeFastTouch)
                    {
                        m_fingerEventState[FINGER_FAST_TOUCH] = true;
                    }

                    m_fingerUpPos.x = event.tfinger.x * Game::instance()->getScreenWidth();
                    m_fingerUpPos.y = Game::instance()->getScreenHeight() - (event.tfinger.y * Game::instance()->getScreenHeight());
                    break;

                case SDL_FINGERMOTION:
                    m_fingerEventState[FINGER_MOTION] = true;

                    m_fingerMotionPos.x = event.tfinger.x * Game::instance()->getScreenWidth();
                    m_fingerMotionPos.y = Game::instance()->getScreenHeight() - (event.tfinger.y * Game::instance()->getScreenHeight());
                    break;
//END TOUCH EVENT

//SCREEN
                case SDL_WINDOWEVENT :
                    if (SDL_GetDisplayOrientation(0) != m_currentOrientation)
                    {
                        SDL_Log("RECREATE WINDOW");

                        Game::instance()->reCreateWindow();

                        m_currentOrientation = SDL_GetDisplayOrientation(0);
                    }


                    break;
//END SCREEN
                default:
                    break;
            }
        }
    }
}

void EventHandler::resetFingerEvents()
{
    traceMethod();

    m_fingerEventState[FINGER_DOWN] = false;
    m_fingerEventState[FINGER_UP] = false;
    m_fingerEventState[FINGER_MOTION] = false;
    m_fingerEventState[FINGER_FAST_TOUCH] = false;
}