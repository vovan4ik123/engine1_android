#ifndef ENGINE1_GAMEOBJECT_H
#define ENGINE1_GAMEOBJECT_H

#include "logs.h"
#include <string>
#include "glm.hpp"
#include "gtx/quaternion.hpp"
#include <map>
#include "assimp/scene.h"
#include <vector>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "UtilityMethods.h"

enum class GameObjectID
{
    UNKNOWN_OBJECT, BUTTON, STATIC_MODEL, MAP_GROUND, ANIMATED_MODEL
};


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textCoords;
};



//Base class for Territory, Barriers, Bushes Trees, Player, Enemies ....
//e.g. all object which interact with each other on scene
class GameObject
{
public:
    virtual ~GameObject() {}

    virtual void update() = 0;
    virtual void draw() = 0;
    virtual void playSound() = 0;

    const GameObjectID getObjectID() const { traceMethod(); return m_objectID; };

    const float getX() const { traceMethod(); return m_posX; }
    const float getY() const { traceMethod(); return m_posY; }
    const float getZ() const { traceMethod(); return m_posZ; }

    const std::string getObjectName() const { traceMethod(); return m_objectName; }

    glm::mat4& getModelMatrixRef() { traceMethod(); return m_modelMatrix; }

protected:
    float m_posX{}, m_posY{}, m_posZ{};
    GameObjectID m_objectID = GameObjectID::UNKNOWN_OBJECT; // initialize in subclass
    std::string m_objectName = ""; // unique name of game object
    glm::mat4 m_modelMatrix{1.0f};
    // bullet does not keep scale component. we need keep it separately
    glm::mat4 m_gameObjectScale{1.0f};
    glm::quat m_gameObjectRotation{1.0f, 0.0f, 0.0f, 0.0f};

    std::string m_directory{};
    std::map<const std::string, aiTextureType> m_texturesMap{};

    // GameObject can has collision object if it participate in physics simulation
    // GameObject store his collision object in Physics.h
    // collisiion object data start

    // collisiion object data end
};

#endif //ENGINE1_GAMEOBJECT_H
