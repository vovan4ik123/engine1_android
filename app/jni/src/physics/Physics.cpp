#include "Physics.h"
#include "SDL.h"
#include "UtilityMethods.h"
#include <vector>

Physics::Physics()
{
    traceMethod();
}

Physics::~Physics()
{
    traceMethod();

    destroyWorld();
}

bool Physics::createWorld()
{
    traceMethod();
    SDL_Log("createWorld()");

    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
    m_overlappingPairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();
    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfig);

    m_world->setGravity(btVector3(0.0f, -10.0f, 0.0f));

    return true;
}

void Physics::simulate()
{
    traceMethod();

    if(nullptr == m_world) Utils::error("Create physics world before use");

    // timeStep: time in sec passed since last m_world->stepSimulation() call
    // maxSubSteps: timeStep < maxSubSteps * fixedTimeStep
    // fixedTimeStep: Inversely proportional to the simulation's resolution.
    // Resolution increases as this value decreases.
    // If your balls penetrates your walls instead of colliding with them decrease it
    m_world->stepSimulation(1.0f/MAX_FPS, 10);
}

bool Physics::destroyWorld()
{
    traceMethod();

    SDL_Log("destroyWorld()");

    m_rigidBodiesMap.clear();

    //cleanup in the reverse order of creation/initialization

    //remove the rigidbodies from the dynamics world and delete them
    for (int i = m_world->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = m_world->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        m_world->removeCollisionObject(obj);
        delete obj;
    }

    //delete collision shapes
    for (int j = 0; j < m_collisionShapes.size(); j++)
    {
        btCollisionShape* shape = m_collisionShapes[j];
        m_collisionShapes[j] = nullptr;
        delete shape;
    }

    m_collisionShapes.clear();

    if(nullptr != m_world) delete m_world;
    m_world = nullptr;
    if(nullptr != m_solver) delete m_solver;
    m_solver = nullptr;
    if(nullptr != m_overlappingPairCache) delete m_overlappingPairCache;
    m_overlappingPairCache = nullptr;
    if(nullptr != m_dispatcher) delete m_dispatcher;
    m_dispatcher = nullptr;
    if(nullptr != m_collisionConfig) delete m_collisionConfig;
    m_collisionConfig = nullptr;

    return true;
}

bool Physics::addMapGround(const std::vector<Vertex>& vert, const std::vector<uint>& ind)
{
    traceMethod();

    btTriangleMesh* triangleMesh = new btTriangleMesh();
    triangleMesh->preallocateVertices(ind.size());
    btVector3 v1, v2, v3;

    for(uint i = 0; i < ind.size(); )
    {
            v1 = btVector3(vert[ind[i]].position.x, vert[ind[i]].position.y, vert[ind[i]].position.z);
            ++i;
            v2 = btVector3(vert[ind[i]].position.x, vert[ind[i]].position.y, vert[ind[i]].position.z);
            ++i;
            v3 = btVector3(vert[ind[i]].position.x, vert[ind[i]].position.y, vert[ind[i]].position.z);
            ++i;

            triangleMesh->addTriangle(v1, v2, v3);
    }

    btCollisionShape* shape = new btBvhTriangleMeshShape(triangleMesh, true);
    m_collisionShapes.push_back(shape);

    btTransform transform;
    transform.setIdentity();

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    float mass = 0.0f;

    btDefaultMotionState* motionState = new btDefaultMotionState(transform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_world->addRigidBody(body);

    return true;
}

bool Physics::addBox(const std::string& objectName, aiVector3D dimensions, aiVector3D pos, aiQuaternion rot, float mass)
{
    traceMethod();

    btCollisionShape* colShape = new btBoxShape(btVector3(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f));
    m_collisionShapes.push_back(colShape);

    btTransform t;
    t.setIdentity();
    t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia{0.0f, 0.0f, 0.0f};
    if (mass != 0.0f)
        colShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    // object does not collide with others objects
    //body->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

    m_rigidBodiesMap.insert(std::make_pair(objectName, body));

    //add body to the dynamics world
    m_world->addRigidBody(body);

    return true;
}

bool Physics::addSphere(const std::string& objectName, float radius, aiVector3D pos, float mass)
{
    traceMethod();

    btCollisionShape* colShape = new btSphereShape(radius);
    m_collisionShapes.push_back(colShape);

    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia{0.0f, 0.0f, 0.0f};
    if (mass != 0.0f) //rigidbody is dynamic if  mass is non zero, otherwise static
        colShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_rigidBodiesMap.insert(std::make_pair(objectName, body));

    //add body to the dynamics world
    m_world->addRigidBody(body);

    return true;
}

bool Physics::addCylinder(const std::string& objectName, aiVector3D dimensions, aiVector3D pos, aiQuaternion rot, float mass)
{
    traceMethod();

    btCollisionShape* colShape = new btCylinderShape(btVector3(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f));
    m_collisionShapes.push_back(colShape);

    btTransform t;
    t.setIdentity();
    t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia{0.0f, 0.0f, 0.0f};
    if (mass != 0.0f)
        colShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_rigidBodiesMap.insert(std::make_pair(objectName, body));

    //add body to the dynamics world
    m_world->addRigidBody(body);

    return true;
}

bool Physics::addCone(const std::string& objectName, float radius, float height, aiVector3D pos, aiQuaternion rot, float mass)
{
    traceMethod();

    btCollisionShape* colShape = new btConeShape(radius, height);
    m_collisionShapes.push_back(colShape);

    btTransform t;
    t.setIdentity();
    t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia{0.0f, 0.0f, 0.0f};
    if (mass != 0.0f)
        colShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_rigidBodiesMap.insert(std::make_pair(objectName, body));

    //add body to the dynamics world
    m_world->addRigidBody(body);

    return true;
}

bool Physics::addCapsule(const std::string& objectName, float radius, float height, aiVector3D pos, aiQuaternion rot, float mass)
{
    traceMethod();

    btCollisionShape* colShape = new btCapsuleShape(radius, height);
    m_collisionShapes.push_back(colShape);

    btTransform t;
    t.setIdentity();
    t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));

    btVector3 localInertia{0.0f, 0.0f, 0.0f};
    if(mass != 0.0f)
        colShape->calculateLocalInertia(mass ,localInertia);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    m_rigidBodiesMap.insert(std::make_pair(objectName, body));

    m_world->addRigidBody(body);

    return true;
}