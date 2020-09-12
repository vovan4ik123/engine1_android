#ifndef ENGINE1_EVENTHANDLER_H
#define ENGINE1_EVENTHANDLER_H

#include <vector>
#include "glm.hpp"
#include "SDL.h"
#include "utility/logs.h"

enum FingerEventID
{
    FINGER_DOWN = 0,
    FINGER_UP = 1,
    FINGER_MOTION = 2,
    FINGER_FAST_TOUCH = 3, //if was touched and released faster than timeFastTouch
};

class EventHandler
{
public:

    void updateEventHandler();
    void resetFingerEvents();

    static EventHandler* instance()
    {
        traceMethod();

        static EventHandler inst;
        return &inst;
    }

    glm::vec2 getFingerDownPos(){ traceMethod(); return m_fingerDownPos;}
    glm::vec2 getFingerUpPos(){ traceMethod(); return m_fingerUpPos;}
    glm::vec2 getFingerMotionPos(){ traceMethod(); return m_fingerMotionPos;}

    bool checkFingerEvent(FingerEventID id) { traceMethod(); return m_fingerEventState[id]; }

private:
    EventHandler();
    ~EventHandler();

    glm::vec2 m_fingerDownPos;
    glm::vec2 m_fingerUpPos;
    glm::vec2 m_fingerMotionPos;

    std::vector<bool> m_fingerEventState;

    unsigned int m_timeFingerDown{};
    unsigned int m_timeFastTouch = 300; // milli sec

    SDL_DisplayOrientation m_currentOrientation;
};


#endif //ENGINE1_EVENTHANDLER_H
