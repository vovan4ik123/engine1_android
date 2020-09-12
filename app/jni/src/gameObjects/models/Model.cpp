#include "Model.h"
#include "Physics.h"
#include "ShaderProgramManager.h"
#include "gtc/type_ptr.hpp"
#include "Camera.h"
#include <sstream>
#include <TextureManager.h>
#include "gtx/matrix_decompose.hpp"

Model::Model(const aiScene* scene, const aiMesh* mesh, std::string path)
{
    traceMethod();
    SDL_Log("Model()");

    m_objectID = GameObjectID::STATIC_MODEL;

    m_objectName = mesh->mName.C_Str();
    m_directory = path.substr(0, path.find_last_of('/'));

    SDL_Log("model %s vertices %i, faces %i", m_objectName.c_str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices{};
    std::vector<GLuint > indices{};

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumVertices);

    // vertices
    for(int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

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

    // init OpenGL
    initGLBuffers(vertices, indices);

    aiMatrix4x4 aiMatr{};
    Utils::findAiMatrixTransform(scene->mRootNode, mesh->mName, aiMatr);
    m_modelMatrix = Utils::aiMatrToGlmMatr(aiMatr);

    aiVector3D scaling{1.0f, 1.0f, 1.0f};
    aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
    aiVector3D translation{0.0f, 0.0f, 0.0f};
    aiMatr.Decompose(scaling, rotation, translation);
    SDL_Log("model rotation w x y z %f %f %f %f ", rotation.w, rotation.x, rotation.y, rotation.z);
    SDL_Log("model scaling x y z %f %f %f ", scaling.x, scaling.y, scaling.z);

    m_gameObjectScale = glm::scale(glm::mat4{1.0f}, glm::vec3{scaling.x, scaling.y, scaling.z});

    initCollisionObject(scene);

    ShaderProgramManager::instance()->makeShaderProgram(ProgramID::MODEL,
                                                        "shaders/models/model.vert",
                                                        "shaders/models/model.frag");

    m_MVP = Camera::instance()->getCamera3D();
}

Model::~Model()
{
    traceMethod();
    SDL_Log("~Model()");
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Model::update()
{
    traceMethod();
}

void Model::draw()
{
    traceMethod();

    updateMVPmatrix();

    glUseProgram(ShaderProgramManager::instance()->getProgram(ProgramID::MODEL));

    glUniformMatrix4fv(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::MODEL), "MVP_matrix"),
                       1, GL_FALSE, glm::value_ptr(m_MVP));

    for(auto iter = m_texturesMap.begin(); iter != m_texturesMap.end(); ++iter)
    {
        if(aiTextureType_DIFFUSE == iter->second)
        {
            // initialize sampler2D unit in shader (sampler2D = integer in shader)
            glUniform1i(glGetUniformLocation(ShaderProgramManager::instance()->getProgram(ProgramID::MODEL),
                                             "material.diffuseTexture"), 0);
            glActiveTexture(GL_TEXTURE0); // then make this unit (sampler2D) active
            glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(iter->first)); // bind texture to this unit
        }
        else if(aiTextureType_SPECULAR == iter->second)
        {
            // initialize sampler2D unit in shader (sampler2D = integer in shader)
            glUniform1i(glGetUniformLocation(ShaderProgramManager::instance()->getProgram(ProgramID::MODEL),
                                             "material.specularTexture"), 1);
            glActiveTexture(GL_TEXTURE1); // then make this unit (sampler2D) active
            glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(iter->first)); // bind texture to this unit
        }
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    for(int i = 0; i < m_texturesMap.size(); ++ i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glUseProgram(0);
}

void Model::playSound()
{
    traceMethod();
}

bool Model::initGLBuffers(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
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

// for m_objectName named model
// find collision object in scene and add corresponding object to physics world
bool Model::initCollisionObject(const aiScene* scene)
{
    traceMethod();

    std::string collisionPartName = std::string(m_objectName + "_collision_");
    aiMesh* collisionMesh = nullptr;
    for(int i = 0; i < scene->mNumMeshes; ++i)
    {
        std::string meshName = scene->mMeshes[i]->mName.C_Str();
        if(meshName.find(collisionPartName) != std::string::npos)
        {
            SDL_Log("found coll mesh %s for %s", meshName.c_str(), m_objectName.c_str());
            collisionMesh = scene->mMeshes[i];
            m_hasCollisionObject = true;
        }
    }

    // m_objectName has not collision object
    if(nullptr == collisionMesh)
    {
        SDL_Log("NO collision mesh for %s", m_objectName.c_str());
        return false;
    }

    std::string collisionFullName = collisionMesh->mName.C_Str();
    if(collisionFullName.find("box_") != std::string::npos)
    {
        collisionPartName += "box_";
        std::istringstream iss(collisionFullName.substr(collisionPartName.size()));
        int mass{};
        iss >> mass;

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        for(int i = 0; i < collisionMesh->mNumVertices; ++i)
        {
            if(collisionMesh->mVertices[i].x > maxX) maxX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].x < minX) minX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].y > maxY) maxY =  collisionMesh->mVertices[i].y;
            if(collisionMesh->mVertices[i].y < minY) minY =  collisionMesh->mVertices[i].y;
            if(collisionMesh->mVertices[i].z > maxZ) maxZ =  collisionMesh->mVertices[i].z;
            if(collisionMesh->mVertices[i].z < minZ) minZ =  collisionMesh->mVertices[i].z;
        }

        aiMatrix4x4 aiMatr{};
        Utils::findAiMatrixTransform(scene->mRootNode, collisionMesh->mName, aiMatr);
        aiVector3D scaling{1.0f, 1.0f, 1.0f};
        aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
        aiVector3D translation{0.0f, 0.0f, 0.0f};
        aiMatr.Decompose(scaling, rotation, translation);

        glm::quat collObjectRotation = glm::quat{rotation.w, rotation.x, rotation.y, rotation.z};
        m_rotationDifference = glm::toMat4(m_gameObjectRotation * glm::inverse(collObjectRotation));

        aiVector3D minXYZ = aiVector3D{minX * scaling.x, minY * scaling.y, minZ * scaling.z};
        aiVector3D maxXYZ = aiVector3D{maxX * scaling.x, maxY * scaling.y, maxZ * scaling.z};

        aiVector3D dimension = maxXYZ - minXYZ;

        SDL_Log("collision box for %s, dimensions %f %f %f, translation %f %f %f, mass %i",
                m_objectName.c_str(), dimension.x, dimension.y, dimension.z, translation.x, translation.y, translation.z, mass);

        return Physics::instance()->addBox(m_objectName, dimension, translation, rotation, mass);
    }
    else if(collisionFullName.find("sphere_") != std::string::npos)
    {
        collisionPartName += "sphere_";
        std::istringstream iss(collisionFullName.substr(collisionPartName.size()));
        int mass{};
        iss >> mass;

        aiMatrix4x4 aiMatr{};
        Utils::findAiMatrixTransform(scene->mRootNode, collisionMesh->mName, aiMatr);
        aiVector3D scaling{1.0f, 1.0f, 1.0f};
        aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
        aiVector3D translation{0.0f, 0.0f, 0.0f};
        aiMatr.Decompose(scaling, rotation, translation);

        glm::quat collObjectRotation = glm::quat{rotation.w, rotation.x, rotation.y, rotation.z};
        m_rotationDifference = glm::toMat4(m_gameObjectRotation * glm::inverse(collObjectRotation));

        aiVector3D radius = collisionMesh->mVertices[0]; // for sphere all vertices are radiuses
        radius.x *= scaling.x;
        radius.y *= scaling.y;
        radius.z *= scaling.z;

        SDL_Log("collision sphere for %s, radius %f, translation %f %f %f, mass %i",
                m_objectName.c_str(), radius.Length() , translation.x, translation.y, translation.z, mass);

        return Physics::instance()->addSphere(m_objectName, radius.Length(), translation, mass);
    }
    else if(collisionFullName.find("cylinder_") != std::string::npos)
    {
        collisionPartName += "cylinder_";
        std::istringstream iss(collisionFullName.substr(collisionPartName.size()));
        int mass{};
        iss >> mass;

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();

        for(int i = 0; i < collisionMesh->mNumVertices; ++i)
        {
            if(collisionMesh->mVertices[i].x > maxX) maxX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].x < minX) minX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].y > maxY) maxY =  collisionMesh->mVertices[i].y;
            if(collisionMesh->mVertices[i].y < minY) minY =  collisionMesh->mVertices[i].y;
            if(collisionMesh->mVertices[i].z > maxZ) maxZ =  collisionMesh->mVertices[i].z;
            if(collisionMesh->mVertices[i].z < minZ) minZ =  collisionMesh->mVertices[i].z;
        }

        aiMatrix4x4 aiMatr{};
        Utils::findAiMatrixTransform(scene->mRootNode, collisionMesh->mName, aiMatr);
        aiVector3D scaling{1.0f, 1.0f, 1.0f};
        aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
        aiVector3D translation{0.0f, 0.0f, 0.0f};
        aiMatr.Decompose(scaling, rotation, translation);

        glm::quat collObjectRotation = glm::quat{rotation.w, rotation.x, rotation.y, rotation.z};
        m_rotationDifference = glm::toMat4(m_gameObjectRotation * glm::inverse(collObjectRotation));

        aiVector3D minXYZ = aiVector3D{minX * scaling.x, minY * scaling.y, minZ * scaling.z};
        aiVector3D maxXYZ = aiVector3D{maxX * scaling.x, maxY * scaling.y, maxZ * scaling.z};

        aiVector3D dimension = maxXYZ - minXYZ;

        SDL_Log("collision cylinder for %s, dimensions %f %f %f, translation %f %f %f, mass %i",
                m_objectName.c_str(), dimension.x, dimension.y, dimension.z, translation.x, translation.y, translation.z, mass);

        return Physics::instance()->addCylinder(m_objectName, dimension, translation, rotation, mass);
    }
    else if(collisionFullName.find("cone_") != std::string::npos)
    {
        collisionPartName += "cone_";
        std::istringstream iss(collisionFullName.substr(collisionPartName.size()));
        int mass{};
        iss >> mass;

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        for(int i = 0; i < collisionMesh->mNumVertices; ++i)
        {
            if(collisionMesh->mVertices[i].x > maxX) maxX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].x < minX) minX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].y > maxY) maxY =  collisionMesh->mVertices[i].y;
            if(collisionMesh->mVertices[i].y < minY) minY =  collisionMesh->mVertices[i].y;
        }

        aiMatrix4x4 aiMatr{};
        Utils::findAiMatrixTransform(scene->mRootNode, collisionMesh->mName, aiMatr);
        aiVector3D scaling{1.0f, 1.0f, 1.0f};
        aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
        aiVector3D translation{0.0f, 0.0f, 0.0f};
        aiMatr.Decompose(scaling, rotation, translation);

        glm::quat collObjectRotation = glm::quat{rotation.w, rotation.x, rotation.y, rotation.z};
        m_rotationDifference = glm::toMat4(m_gameObjectRotation * glm::inverse(collObjectRotation));

        float radius = (maxX * scaling.x - minX * scaling.x) /2;
        float height = maxY * scaling.y - minY * scaling.y;

        SDL_Log("collision cone for %s, radius %f height %f ,translation %f %f %f, mass %i",
                m_objectName.c_str(), radius, height, translation.x, translation.y, translation.z, mass);

        SDL_Log("coll cone rotation w x y z %f %f %f %f ", rotation.w, rotation.x, rotation.y, rotation.z);
        SDL_Log("coll cone scaling x y z %f %f %f ", scaling.x, scaling.y, scaling.z);

        Physics::instance()->addCone(m_objectName, radius, height, translation, rotation, mass);
    }
    else if(collisionFullName.find("capsule_") != std::string::npos)
    {
        collisionPartName += "capsule_";
        std::istringstream iss(collisionFullName.substr(collisionPartName.size()));
        int mass{};
        iss >> mass;

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();

        for(int i = 0; i < collisionMesh->mNumVertices; ++i)
        {
            if(collisionMesh->mVertices[i].x > maxX) maxX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].x < minX) minX =  collisionMesh->mVertices[i].x;
            if(collisionMesh->mVertices[i].y > maxY) maxY =  collisionMesh->mVertices[i].y;
            if(collisionMesh->mVertices[i].y < minY) minY =  collisionMesh->mVertices[i].y;
        }

        aiMatrix4x4 aiMatr{};
        Utils::findAiMatrixTransform(scene->mRootNode, collisionMesh->mName, aiMatr);
        aiVector3D scaling{1.0f, 1.0f, 1.0f};
        aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
        aiVector3D translation{0.0f, 0.0f, 0.0f};
        aiMatr.Decompose(scaling, rotation, translation);

        glm::quat collObjectRotation = glm::quat{rotation.w, rotation.x, rotation.y, rotation.z};
        m_rotationDifference = glm::toMat4(m_gameObjectRotation * glm::inverse(collObjectRotation));

        float radius = (maxX * scaling.x - minX * scaling.x) /2;
        float height = (maxY * scaling.y - minY * scaling.y) - radius * 2; // height without caps (cylinder)
        if(height <= 0.0f) Utils::error("collision capsule MUST have half of spheres at the ends. "
                                       "Probably you scaled capsule only along 1 or 2 axis", collisionFullName);

        SDL_Log("collision capsule for %s, radius %f total height %f , translation %f %f %f, mass %i",
                m_objectName.c_str(), radius, height + radius * 2, translation.x, translation.y, translation.z, mass);

        Physics::instance()->addCapsule(m_objectName, radius, height, translation, rotation, mass);
    }

    return true;
}

void Model::updateMVPmatrix()
{
    traceMethod();

    if(m_hasCollisionObject)
    {
        // m_modelMatrix was changes by bullet :
        // 1. bullet does not keep scale component in matrix
        // 2. if gameObj originally was rotated not same as collisionObg we should add difference
        m_MVP = Camera::instance()->getCamera3D() * m_modelMatrix * m_rotationDifference * m_gameObjectScale;
    }
    else
    {
        m_MVP = Camera::instance()->getCamera3D() * m_modelMatrix;
    }
/*
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::quat rot{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 trans{0.0f, 0.0f, 0.0f};
    glm::vec3 scew{0.0f, 0.0f, 0.0f};
    glm::vec4 persp{0.0f, 0.0f, 0.0f, 0.0f};
    glm::decompose(m_MVP,scale, rot, trans, scew, persp);

    SDL_Log("m_gameObjectScale x y z %f %f %f", m_gameObjectScale[0][0], m_gameObjectScale[1][1], m_gameObjectScale[2][2]);

    SDL_Log("m_MVP scaling x y z %f %f %f", scale.x, scale.y, scale.z);
*/
}