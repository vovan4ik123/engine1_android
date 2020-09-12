#ifndef ENGINE1_MAPGROUND_H
#define ENGINE1_MAPGROUND_H

#include "GameObject.h"

class MapGround : public GameObject
{
public:
    MapGround(const aiScene* scene, const aiMesh* mesh, std::string path);
    ~MapGround();

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

private:
    std::string m_directory{};

    glm::mat4 m_MVP{};

    bool initGLBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    GLuint m_VAO{};
    GLuint m_VBO{};
    GLuint m_EBO{};
    GLuint m_indicesCount{};

    bool initCollisionObject(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

    float m_groundStartX = std::numeric_limits<float>::max();
    float m_groundStartZ = std::numeric_limits<float>::max();
    float m_groundEndX = std::numeric_limits<float>::min();
    float m_groundEndZ = std::numeric_limits<float>::min();

};


#endif //ENGINE1_MAPGROUND_H
