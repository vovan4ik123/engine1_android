#ifndef ENGINE1_PHYSICS_H
#define ENGINE1_PHYSICS_H

#include "utility/logs.h"
#include "btBulletDynamicsCommon.h"
#include <map>
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "GameObject.h"
#include <vector>

extern const int MAX_FPS;

class Physics
{
public:
    static Physics* instance()
    {
        traceMethod();

        static Physics inst;
        return &inst;
    }


    void getTransformMatrix(const std::string& name, glm::mat4& matr)
    {
        traceMethod();

        std::map<const std::string, btRigidBody*>::iterator iter = m_rigidBodiesMap.find(name);
        if(iter != m_rigidBodiesMap.end())
        {
            btRigidBody* body = iter->second;
            btTransform t;

            if(body->getMotionState()) body->getMotionState()->getWorldTransform(t);
            else t = body->getWorldTransform();

            t.getOpenGLMatrix(const_cast<float*>(glm::value_ptr(matr)));
        }
    }

    void setTransformMatrix(const std::string& name, glm::mat4 matr, bool resetVelocities = true)
    {
        traceMethod();

        std::map<const std::string, btRigidBody*>::iterator iter = m_rigidBodiesMap.find(name);
        if(iter != m_rigidBodiesMap.end())
        {
            btRigidBody* body = iter->second;

            btTransform t;
            t.setFromOpenGLMatrix(glm::value_ptr(matr));

            body->setWorldTransform(t);
            if(resetVelocities)
            {
                body->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
                body->setAngularVelocity(btVector3(0.0f, 0.0f, 0.0f));
                body->clearForces();
            }

            if(body->getMotionState())
                body->getMotionState()->setWorldTransform(t);

            for(iter = m_rigidBodiesMap.begin(); iter != m_rigidBodiesMap.end(); ++iter)
            {
                if(iter->second->getActivationState() != ACTIVE_TAG)
                    iter->second->activate(true);
            }
        }
    }

    void eraseFromWorld(const std::string& name)
    {
        traceMethod();

        std::map<const std::string, btRigidBody *>::iterator iter = m_rigidBodiesMap.find(name);
        if (iter != m_rigidBodiesMap.end())
        {
            btRigidBody *body = iter->second;
            if (body->getMotionState()) delete body->getMotionState();

            m_world->removeCollisionObject(body);
            delete body;

            m_rigidBodiesMap.erase(iter);

            for(iter = m_rigidBodiesMap.begin(); iter != m_rigidBodiesMap.end(); ++iter)
            {
                if(iter->second->getActivationState() != ACTIVE_TAG)
                    iter->second->activate(true);
            }
        }
    }

    bool createWorld();
    void simulate();
    bool destroyWorld();

    // functions to add collision shapes to physics world
    bool addMapGround(const std::vector<Vertex>& vert, const std::vector<uint>& ind);
    bool addBox(const std::string& objectName, aiVector3D dimensions, aiVector3D pos, aiQuaternion rot, float mass);
    bool addSphere(const std::string& objectName, float radius, aiVector3D pos, float mass);
    bool addCylinder(const std::string& objectName, aiVector3D dimensions, aiVector3D pos, aiQuaternion rot, float mass);
    bool addCone(const std::string& objectName, float radius, float height, aiVector3D pos, aiQuaternion rot, float mass);
    bool addCapsule(const std::string& objectName, float radius, float height, aiVector3D pos, aiQuaternion rot, float mass);

private:
    Physics();
    ~Physics();

    btDefaultCollisionConfiguration* m_collisionConfig{};
    btCollisionDispatcher* m_dispatcher{};
    btBroadphaseInterface* m_overlappingPairCache{};
    btSequentialImpulseConstraintSolver* m_solver{};
    btDiscreteDynamicsWorld* m_world{};

    // keep track of the shapes, we release memory at exit.
    btAlignedObjectArray<btCollisionShape*> m_collisionShapes{};

    // bodies MUST have
    std::map<const std::string, btRigidBody*> m_rigidBodiesMap{};
};


#endif //ENGINE1_PHYSICS_H
