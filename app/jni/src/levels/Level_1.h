#ifndef ENGINE1_LEVEL_1_H
#define ENGINE1_LEVEL_1_H

#include "GameLevel.h"
#include "GameObject.h"
#include "memory"
#include <vector>
#include "Model.h"
#include <array>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

extern uint FPS;
extern uint fTime;

class Level_1 : public GameLevel
{
public:
    Level_1();
    virtual ~Level_1();

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

private:
    struct ImporterScene
    {
        Assimp::Importer importer{};
        const aiScene* scene{};
    };
    // keep all scenes in vector. free only when level id destruct. Animation data is stored in scene
    constexpr static uint MAX_SCENES = 50;
    std::array<ImporterScene, MAX_SCENES> m_importerScene;
    uint m_importerSceneIndex{};

    std::vector<std::shared_ptr<GameObject>> m_gameObjects;

    void loadScene(std::string path);
    void showNodesName(aiNode* node);
    // one file ....dae = one scene
    // one mesh in scene = one static model
    void processScene(const aiScene* scene, std::string path);

    // camera drag for tests
    glm::vec2 lastDragPos{0.0f};
    glm::vec2 offsets{0.0f};
    glm::vec2 motionPos{0.0f};
};

#endif //ENGINE1_LEVEL_1_H
