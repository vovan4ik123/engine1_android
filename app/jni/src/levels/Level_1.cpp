#include "Level_1.h"
#include "SDL.h"
#include "Button.h"
#include "Axis.h"
#include "PlayState.h"
#include "Camera.h"
#include "UtilityMethods.h"
#include "MapGround.h"
#include <chrono>
#include <EventHandler.h>
#include "Physics.h"
#include "Text.h"
#include "AnimatedModel.h"

Level_1::Level_1()
{
    traceMethod();
    SDL_Log("Level_1()");

    m_levelCompleted = false;
    m_levelFailed = false;

    m_ID = GameLevelID::LEVEL_1;

    auto timeStart = std::chrono::system_clock::now();

    // create physics world FIRST !!!!!!!!!!
    Physics::instance()->createWorld();
    Text::instance(); // create object

    std::shared_ptr<Button> pauseButton = std::make_shared<Button>(100.0f, 300.0f, 1.0f, 200.0f, 100.0f, "images/pause.png");
    pauseButton->setButtonAction(&PlayState::playToPause);
    m_gameObjects.push_back(pauseButton);

    std::shared_ptr<Axis> axis = std::make_shared<Axis>();
    m_gameObjects.push_back(axis);

    // scene 1 load
    loadScene("models/a1.fbx");
    loadScene("models/enemy/model.dae");
    loadScene("models/level1/big_ground.dae");
    loadScene("models/level1/collisions_5.dae");

    //loadScene("models/level1/canion.dae");
    //loadScene("models/level1/cone_cube.dae");


    auto timeEnd = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedSeconds = timeEnd - timeStart;
    SDL_Log("load Level 1 time %f", elapsedSeconds.count());
}

Level_1::~Level_1()
{
    traceMethod();
    SDL_Log("~Level_1()");

    Physics::instance()->destroyWorld();

    for(std::array<ImporterScene, MAX_SCENES>::iterator iter = m_importerScene.begin(); iter != m_importerScene.end(); ++iter)
    {
        iter->importer.FreeScene();
    }
}

void Level_1::loadScene(std::string path)
{
    uint resSize{};
    const char* res = Utils::readFileToBuffer(path.c_str(), &resSize);
    if(!res) Utils::error("readFileToBuffer() return nullptr for", path);

    m_importerScene.at(m_importerSceneIndex).scene =
            m_importerScene.at(m_importerSceneIndex).importer.ReadFileFromMemory(res, resSize,
                                                                                      aiProcess_Triangulate |
                                                                                      aiProcess_SortByPType |
                                                                                      aiProcess_FlipUVs);
    delete[] res;

    if( !m_importerScene.at(m_importerSceneIndex).scene ||
        !m_importerScene.at(m_importerSceneIndex).scene->mRootNode ||
        m_importerScene.at(m_importerSceneIndex).scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE)
        Utils::error("scene importer return nullptr ", path);

    SDL_Log("nodes name in scene %s", path.c_str());
    showNodesName(m_importerScene.at(m_importerSceneIndex).scene->mRootNode);

    SDL_Log("mNumMeshes: %i", m_importerScene.at(m_importerSceneIndex).scene->mNumMeshes);
    SDL_Log("load meshes into objects");
    // parse scene and load 3D models
    processScene(m_importerScene.at(m_importerSceneIndex).scene, path);

    ++m_importerSceneIndex;
}

void Level_1::showNodesName(aiNode* node)
{
    SDL_Log(" - %s", node->mName.C_Str());
    for (uint i = 0; i < node->mNumChildren; i++)
    {
        showNodesName(node->mChildren[i]);
    }
}

void Level_1::processScene(const aiScene* scene, std::string path)
{
    traceMethod();

    aiMesh* mesh;
    std::shared_ptr<GameObject> gameObject;
    for(int i = 0; i < scene->mNumMeshes; ++i)
    {
        mesh = scene->mMeshes[i];
        std::string meshName = mesh->mName.C_Str();

        if(meshName.find("ground") != std::string::npos)
        {
            gameObject = std::make_shared<MapGround>(scene, mesh, path);
            m_gameObjects.push_back(gameObject);
        }
        else if(meshName.find("_collision") == std::string::npos && !mesh->HasBones())
        {       // load meshes for drawing not marked as collision
            gameObject = std::make_shared<Model>(scene, mesh, path); // 1 mesh = 1 static model
            m_gameObjects.push_back(gameObject);
        }
        else if(meshName.find("_collision") == std::string::npos && mesh->HasBones())
        {       // load meshes for drawing not marked as collision + has bones
            gameObject = std::make_shared<AnimatedModel>(scene, mesh, path);
            m_gameObjects.push_back(gameObject);
        }

    }
}

void Level_1::update()
{
    traceMethod();

    // 1. update all object
    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->update();
    }

    if(SDL_GetTicks() % 15000 > 14920)
    {
        float x = rand() % 300 - 150.0f;
        float z = rand() % 300 - 150.0f;
        glm::mat4 m = glm::mat4(1.0f);
        Physics::instance()->getTransformMatrix("Suzanne", m);
        m[3].x = x; m[3].y = 50.0f; m[3].z = z;
        Physics::instance()->setTransformMatrix("Suzanne", m);

        Physics::instance()->getTransformMatrix("Tank", m);
        m[3].x = x; m[3].y = 55.0f; m[3].z = z;
        Physics::instance()->setTransformMatrix("Tank", m);

        Physics::instance()->getTransformMatrix("Capsule1", m);
        m[3].x = x; m[3].y = 60.0f; m[3].z = z;
        Physics::instance()->setTransformMatrix("Capsule1", m);

        Physics::instance()->getTransformMatrix("Gear", m);
        m[3].x = x; m[3].y = 65.0f; m[3].z = z;
        Physics::instance()->setTransformMatrix("Gear", m);

        Physics::instance()->getTransformMatrix("Bolt", m);
        m[3].x = x; m[3].y = 70.0f; m[3].z = z;
        Physics::instance()->setTransformMatrix("Bolt", m);
    }

    // 2. simulate physics based on given objects
    Physics::instance()->simulate();

    // 3. set new model matrices to objects after physics simulation
    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        if(m_gameObjects[i]->getObjectName() != "")
        {
            //SDL_Log("update matrix for %s", m_gameObjects[i]->getObjectName().c_str());
            Physics::instance()->getTransformMatrix(m_gameObjects[i]->getObjectName(),
                                                    m_gameObjects[i]->getModelMatrixRef());
        }
    }

    if(EventHandler::instance()->checkFingerEvent(FingerEventID::FINGER_DOWN))
    {
        motionPos = motionPos = EventHandler::instance()->getFingerDownPos();
        lastDragPos = motionPos;
    }
    if(EventHandler::instance()->checkFingerEvent(FingerEventID::FINGER_MOTION))
    {
        motionPos = EventHandler::instance()->getFingerMotionPos();

        offsets += (motionPos - lastDragPos) / 4.0f;

        lastDragPos = motionPos;
    }

    glm::mat4 tm = glm::mat4(1.0f);
    Physics::instance()->getTransformMatrix("Bolt", tm);
    Camera::instance()->setCameraPos(glm::vec3(glm::cos(glm::radians(offsets.x)) * 55.0f + tm[3].x,
                                               tm[3].y -offsets.y / 4.0f,
                                               glm::sin(glm::radians(offsets.x)) * 55.0f  + tm[3].z));
    Camera::instance()->setCameraFront(glm::vec3(tm[3].x,tm[3].y, tm[3].z));
/*
    Camera::instance()->setCameraPos(glm::vec3(glm::cos(glm::radians(offsets.x)) * 25.0f,
                                               -offsets.y / 4.0f,
                                               glm::sin(glm::radians(offsets.x)) * 25.0f));
    Camera::instance()->setCameraFront(glm::vec3(0.0f, 0.0f, 0.0f));
*/
}

void Level_1::draw()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->draw();
    }
    std::string FPSinfo = "FPS: "; FPSinfo += std::to_string(FPS);
    FPSinfo += ".  FrameTime: "; FPSinfo += std::to_string(fTime);
    int fpsYpos = Game::instance()->getScreenHeight() - 110;
    Text::instance()->draw2Dtext(FontID::ROBOTO, FPSinfo, glm::vec3(10.0f, fpsYpos, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f);

    Text::instance()->draw2Dtext(FontID::ROBOTO, "FontID::ROBOTO.,<>=!&7?123 Q:; +-=", glm::vec3(100.0f, 100.0f, 0.3f), glm::vec3(7.0f, 0.2f, 0.0f), 1.0f, 1.0f);
    Text::instance()->draw2Dtext(FontID::ROBOTO_ITALIC, "FontID::ROBOTO_ITALIC.,<>=!&7?123 Q:; +-=", glm::vec3(200.0f, 300.0f, 0.3f), glm::vec3(1.0f, 0.6f, 0.0f), 1.5f, 0.5f);
    Text::instance()->draw2Dtext(FontID::CREAMY, "FontIDqwe::CREAMY !@#$%^&*()123 -=+", glm::vec3(100.0f, 700.0f, 0.3f), glm::vec3(1.0f, 1.0f, 0.2f), 0.7f, 1.0f);

}

void Level_1::playSound()
{
    traceMethod();

    for(int i = 0; i < m_gameObjects.size(); ++i)
    {
        m_gameObjects[i]->playSound();
    }
}
