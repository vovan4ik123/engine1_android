#ifndef ENGINE1_LOGS_H
#define ENGINE1_LOGS_H

#include "../../SDL/include/SDL.h"
#include <string>
#include <sstream>

#ifdef TRACE_LVL_20
    #define TRACE_METHOD
    #define TRACE_WARNING
    #define TRACE_INFO
#endif //TRACE_LVL_20

#ifdef TRACE_LVL_10
    #define TRACE_WARNING
    #define TRACE_INFO
#endif //TRACE_LVL_10

#ifdef TRACE_LVL_5
    #define TRACE_INFO
#endif //TRACE_LVL_5

#ifdef TRACE_METHOD
    #define traceMethod(...)                        \
        {                                           \
            std::string macro_fnName = __FILE__;                                    \
            macro_fnName = macro_fnName.substr(macro_fnName.find_last_of("/")+1);   \
            for(; macro_fnName.length() < 30;) macro_fnName += " ";                 \
            macro_fnName += std::to_string(__LINE__);                               \
            for(; macro_fnName.length() < 40;) macro_fnName += " ";                 \
            macro_fnName += __FUNCTION__; macro_fnName += "()";                     \
            SDL_Log("TRACE_METHOD: %s ", macro_fnName.c_str());                     \
        }

#else
    #define traceMethod(...)
#endif //TRACE_METHOD

#ifdef TRACE_WARNING

#define traceWarning_0()                            \
        {                                           \
            std::string macro_fnName = __FILE__;                                \
            macro_fnName = macro_fnName.substr(macro_fnName.find_last_of("/")+1);\
            for(; macro_fnName.length() < 30;) macro_fnName += " ";             \
            macro_fnName += std::to_string(__LINE__);                           \
            for(; macro_fnName.length() < 40;) macro_fnName += " ";             \
            macro_fnName += __FUNCTION__; macro_fnName += "()  warning: ";      \
            SDL_Log("TRACE_WARNING: %s ", macro_fnName.c_str());                \
        }

#define traceWarning_1(A)                           \
        {                                           \
            std::string macro_fnName = __FILE__;                                \
            macro_fnName = macro_fnName.substr(macro_fnName.find_last_of("/")+1);\
            for(; macro_fnName.length() < 30;) macro_fnName += " ";             \
            macro_fnName += std::to_string(__LINE__);                           \
            for(; macro_fnName.length() < 40;) macro_fnName += " ";             \
            macro_fnName += __FUNCTION__; macro_fnName += "()  warning: ";      \
            std::ostringstream  macro_oss; macro_oss.str("");                   \
            macro_oss << A; macro_fnName += macro_oss.str();                    \
            SDL_Log("TRACE_WARNING: %s ", macro_fnName.c_str());                \
        }

#define traceWarning_2(A, B)                        \
        {                                           \
            std::string macro_fnName = __FILE__;                                \
            macro_fnName = macro_fnName.substr(macro_fnName.find_last_of("/")+1);\
            for(; macro_fnName.length() < 30;) macro_fnName += " ";             \
            macro_fnName += std::to_string(__LINE__);                           \
            for(; macro_fnName.length() < 40;) macro_fnName += " ";             \
            macro_fnName += __FUNCTION__; macro_fnName += "()  warning: ";      \
            std::ostringstream  macro_oss; macro_oss.str("");                   \
            macro_oss << A << " " << B; macro_fnName += macro_oss.str();        \
            SDL_Log("TRACE_WARNING: %s ", macro_fnName.c_str());                \
        }


#define traceWarning_3(A, B, C)                     \
        {                                           \
            std::string macro_fnName = __FILE__;                                \
            macro_fnName = macro_fnName.substr(macro_fnName.find_last_of("/")+1);\
            for(; macro_fnName.length() < 30;) macro_fnName += " ";             \
            macro_fnName += std::to_string(__LINE__);                           \
            for(; macro_fnName.length() < 40;) macro_fnName += " ";             \
            macro_fnName += __FUNCTION__; macro_fnName += "()  warning: ";      \
            std::ostringstream  macro_oss; macro_oss.str("");                   \
            macro_oss << A << " " << B << " " << C; macro_fnName += macro_oss.str();\
            SDL_Log("TRACE_WARNING: %s ", macro_fnName.c_str());                \
        }


#define traceWarning_4(A, B, C, D)                  \
        {                                           \
            std::string macro_fnName = __FILE__;                                \
            macro_fnName = macro_fnName.substr(macro_fnName.find_last_of("/")+1);\
            for(; macro_fnName.length() < 30;) macro_fnName += " ";             \
            macro_fnName += std::to_string(__LINE__);                           \
            for(; macro_fnName.length() < 40;) macro_fnName += " ";             \
            macro_fnName += __FUNCTION__; macro_fnName += "()  warning: ";      \
            std::ostringstream  macro_oss; macro_oss.str("");                   \
            macro_oss << A << " " << B << " " << C << " " << D; macro_fnName += macro_oss.str();\
            SDL_Log("TRACE_WARNING: %s ", macro_fnName.c_str());                \
        }
// correct macro chooser (0 - 4 arguments)
#define traceWarning_X(x,A,B,C,D,FUNC, ...)  FUNC

// macro for programmer
#define traceWarning(...)   traceWarning_X(,##__VA_ARGS__,\
                                               traceWarning_4(__VA_ARGS__),\
                                               traceWarning_3(__VA_ARGS__),\
                                               traceWarning_2(__VA_ARGS__),\
                                               traceWarning_1(__VA_ARGS__),\
                                               traceWarning_0(__VA_ARGS__) )

#else
    #define traceWarning(...)
#endif //TRACE_WARNING

#endif //ENGINE1_LOGS_H
