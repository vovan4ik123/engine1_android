#include "MapGround.h"
#include <limits>
#include <math.h>
#include "UtilityMethods.h"
#include "Camera.h"
#include "ShaderProgramManager.h"
#include "gtc/type_ptr.hpp"
#include "Physics.h"
#include "TextureManager.h"

MapGround::MapGround(const aiScene* scene, const aiMesh* mesh, std::string path)
{
    traceMethod();
    SDL_Log("MapGround()");

    m_objectID = GameObjectID::MAP_GROUND;

    m_objectName = mesh->mName.C_Str();
    m_directory = path.substr(0, path.find_last_of('/'));

    SDL_Log("model %s vertices %i, faces %i", m_objectName.c_str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices{};
    std::vector<GLuint > indices{};

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);

    aiMatrix4x4 aiMatr{};
    Utils::findAiMatrixTransform(scene->mRootNode, mesh->mName, aiMatr);
    m_modelMatrix = Utils::aiMatrToGlmMatr(aiMatr);

    // vertices
    for(int i = 0; i < mesh->mNumVertices; ++i)
    {
        // find start and end rows X Z
        if (mesh->mVertices[i].x < m_groundStartX) m_groundStartX = mesh->mVertices[i].x;
        if (mesh->mVertices[i].z < m_groundStartZ) m_groundStartZ = mesh->mVertices[i].z;
        if (mesh->mVertices[i].x > m_groundEndX) m_groundEndX = mesh->mVertices[i].x;
        if (mesh->mVertices[i].z > m_groundEndZ) m_groundEndZ = mesh->mVertices[i].z;

        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        vertex.position = m_modelMatrix * glm::vec4(vertex.position, 1.0f);

        if(NULL != mesh->mNormals)
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }
        else
        {
            vertex.normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // use only first set of texture coordinates
        if(mesh->mTextureCoords[0])
        {
            vertex.textCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.textCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else
        {
            vertex.textCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // indices
    for(int i = 0; i < mesh->mNumFaces; ++i) // every face MUST be a triangle !!!!
    {
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }


    // material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        if(material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            aiString filename;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
            std::string fullpath = m_directory + '/' + filename.C_Str();
            m_texturesMap.insert(std::make_pair(fullpath, aiTextureType_DIFFUSE));
            TextureManager::instance()->load_PNG_or_JPEG(fullpath.c_str());
        }
        if(material->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString filename;
            material->GetTexture(aiTextureType_SPECULAR, 0, &filename);
            std::string fullpath = m_directory + '/' + filename.C_Str();
            m_texturesMap.insert(std::make_pair(fullpath, aiTextureType_SPECULAR));
            TextureManager::instance()->load_PNG_or_JPEG(fullpath.c_str());
        }
    }

    m_groundStartX = round(m_groundStartX);
    m_groundStartZ = round(m_groundStartZ);
    m_groundEndZ = round(m_groundEndZ);
    m_groundEndX = round(m_groundEndX);

    initGLBuffers(vertices, indices);

    initCollisionObject(vertices, indices);

    // create shaders
    ShaderProgramManager::instance()->makeShaderProgram(ProgramID::GROUND,
                                                        "shaders/mapGround/ground.vert",
                                                        "shaders/mapGround/ground.frag");

    m_MVP = Camera::instance()->getCamera3D();
}

MapGround::~MapGround()
{
    traceMethod();
    SDL_Log("~MapGround()");

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void MapGround::update()
{
    traceMethod();

}

void MapGround::draw()
{
    traceMethod();

    m_MVP = Camera::instance()->getCamera3D();

    glUseProgram(ShaderProgramManager::instance()->getProgram(ProgramID::GROUND));

    glUniformMatrix4fv(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::GROUND), "MVP_matrix"),
                       1, GL_FALSE, glm::value_ptr(m_MVP));

    for(auto iter = m_texturesMap.begin(); iter != m_texturesMap.end(); ++iter)
    {
        if(aiTextureType_DIFFUSE == iter->second)
        {
            // initialize sampler2D unit in shader (sampler2D = integer in shader)
            glUniform1i(glGetUniformLocation(ShaderProgramManager::instance()->getProgram(ProgramID::GROUND),
                                             "material.diffuseTexture"), 0);
            glActiveTexture(GL_TEXTURE0); // then make this unit (sampler2D) active
            glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(iter->first)); // bind texture to this unit
        }
        else if(aiTextureType_SPECULAR == iter->second)
        {
            // initialize sampler2D unit in shader (sampler2D = integer in shader)
            glUniform1i(glGetUniformLocation(ShaderProgramManager::instance()->getProgram(ProgramID::GROUND),
                                             "material.specularTexture"), 1);
            glActiveTexture(GL_TEXTURE1); // then make this unit (sampler2D) active
            glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(iter->first)); // bind texture to this unit
        }
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_LINES, m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    for(int i = 0; i < m_texturesMap.size(); ++ i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);
}

void MapGround::playSound()
{
    traceMethod();
}

bool MapGround::initGLBuffers(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    traceMethod();

    m_indicesCount = indices.size();

    // buffer data
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // numbers of vertices
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indicesCount * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, textCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBindVertexArray(0);

    return true;
}

//bool MapGround::initCollisionObject(const aiMesh* mesh)
bool MapGround::initCollisionObject(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
{
    traceMethod();

    return Physics::instance()->addMapGround(vertices, indices);
}