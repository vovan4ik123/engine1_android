#ifndef ENGINE1_MODEL_H
#define ENGINE1_MODEL_H

#include "GameObject.h"

class Model : public GameObject
{
public:
    Model(const aiScene* scene, const aiMesh* mesh, std::string path);
    ~Model();

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

private:
    glm::mat4 m_MVP{1.0f};
    void updateMVPmatrix();

    bool initGLBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    GLuint m_VAO{};
    GLuint m_VBO{};
    GLuint m_EBO{};
    GLuint m_indicesCount{};

    bool m_hasCollisionObject = false; // if true m_modelMatrix will be changed by bullet
    glm::mat4 m_rotationDifference{1.0f}; // difference between rotation gameObj and collisionObj
                                            // in case when collisionObj was rotated not as gameObj
    bool initCollisionObject(const aiScene* scene);
};


#endif //ENGINE1_MODEL_H
