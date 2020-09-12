LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main


SDL_PATH := ../SDL
SDL_IMAGE_PATH := ../SDL_image
SDL_MIXER_PATH := ../SDL_mixer
GLM_PATH := ../glm
FREETYPE_PATH := ../freetype/include
SQLITE_PATH := ../sqlite
ASSIMP_PATH := ../assimp/include
BULLET_PATH := ../bullet/src

# libraries headers
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include 	\
					$(LOCAL_PATH)/$(SDL_IMAGE_PATH)		\
					$(LOCAL_PATH)/$(SDL_MIXER_PATH)		\
					$(LOCAL_PATH)/$(GLM_PATH)			\
					$(LOCAL_PATH)/$(SQLITE_PATH)		\
					$(LOCAL_PATH)/$(ASSIMP_PATH)        \
					$(LOCAL_PATH)/$(BULLET_PATH)        \
					$(LOCAL_PATH)/$(FREETYPE_PATH)      \

# source code headers
LOCAL_C_INCLUDES += $(LOCAL_PATH)/camera   \
                    $(LOCAL_PATH)/gameObjects   \
                    $(LOCAL_PATH)/gameObjects/models   \
                    $(LOCAL_PATH)/gameStates   \
                    $(LOCAL_PATH)/levels   \
                    $(LOCAL_PATH)/physics   \
                    $(LOCAL_PATH)/text  \
                    $(LOCAL_PATH)/utility   \

# Add your application source files here...
LOCAL_SRC_FILES := main.cpp \
                   Game.cpp \
                   GameStateMachine.cpp \
                   EventHandler.cpp \
                   utility/UtilityMethods.cpp \
                   utility/ShaderProgramManager.cpp   \
                   utility/TextureManager.cpp   \
                   gameStates/MainMenuState.cpp    \
                   gameStates/PlayState.cpp    \
                   gameStates/PauseState.cpp   \
                   gameObjects/Button.cpp   \
                   gameObjects/Axis.cpp    \
                   gameObjects/MapGround.cpp    \
                   levels/Level_1.cpp   \
                   gameObjects/models/Model.cpp   \
                   gameObjects/models/AnimatedModel.cpp \
                   physics/Physics.cpp  \
                   text/Text.cpp

# LOCAL_CFLAGS := -DTRACE_LVL_20
LOCAL_CFLAGS := -DTRACE_LVL_10

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_mixer sqlite freetype assimp bulletPhysics

LOCAL_LDLIBS := -lGLESv3 -llog

include $(BUILD_SHARED_LIBRARY)
