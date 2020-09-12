#ifndef ENGINE1_UTILITYMETHODS_H
#define ENGINE1_UTILITYMETHODS_H

#include "GameLevel.h"
#include <sstream>
#include <string>
#include <assimp/scene.h>
#include "glm.hpp"

namespace Utils
{
    // This fn() call new[] !!! You must call delete[] for free buffer.
    char* readFileToBuffer(const char* filename, uint* size = nullptr);

    bool writeFileToInternalStorageAndroid(const char* filename, const char* buffer, uint size);

    // GameLevelID to int ===> int(GameLevelID::LEVEL_1)
    GameLevelID intToLevelID(int x);

    void findAiMatrixTransform(const aiNode* node, aiString meshName, aiMatrix4x4& matrToSet);
    // assimp matrix is row-major -> glm matrix is column-major
    glm::mat4 aiMatrToGlmMatr(aiMatrix4x4 matrix);

    void error(const std::string& s);

    void error(const std::string& s1, const std::string& s2);

    void error(const std::string& s, const double d);
}

#endif //ENGINE1_UTILITYMETHODS_H
