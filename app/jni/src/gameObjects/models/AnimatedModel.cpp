#include "AnimatedModel.h"
#include "TextureManager.h"
#include "ShaderProgramManager.h"
#include "gtx/matrix_decompose.hpp"
#include "Camera.h"
#include "gtc/type_ptr.hpp"
#include "EventHandler.h"

AnimatedModel::AnimatedModel(const aiScene* scene, const aiMesh* mesh, std::string path)
{
    traceMethod();
    SDL_Log("AnimatedModel()");

    m_scene = scene;

    // LOGS START

    SDL_Log("m_scene->mNumAnimations %i", m_scene->mNumAnimations);
    for(int i = 0; i < m_scene->mNumAnimations; ++i)
    {
        SDL_Log("m_scene->mNumAnimations[i]->mName %s", m_scene->mAnimations[i]->mName.C_Str());
        SDL_Log("m_scene->mNumAnimations[i]->mTicksPerSecond %f", m_scene->mAnimations[i]->mTicksPerSecond);
        SDL_Log("m_scene->mNumAnimations[i]->mDuration %f", m_scene->mAnimations[i]->mDuration);

        SDL_Log("mNumPositionKeys %i", m_scene->mAnimations[i]->mChannels[0]->mNumPositionKeys);
        SDL_Log("mNumRotationKeys %i", m_scene->mAnimations[i]->mChannels[0]->mNumRotationKeys);
        SDL_Log("mScalingKeys %i", m_scene->mAnimations[i]->mChannels[0]->mNumScalingKeys);

        for(int j = 0; j < m_scene->mAnimations[i]->mChannels[0]->mNumRotationKeys; ++j)
        {
            SDL_Log("key %i: time: %f", j, m_scene->mAnimations[i]->mChannels[0]->mRotationKeys[j].mTime);
        }

        SDL_Log("animated nodes: %i", m_scene->mAnimations[i]->mNumChannels);
        for (int j = 0; j < m_scene->mAnimations[i]->mNumChannels; ++j)
        {
            SDL_Log("%s", m_scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str());
        }

        SDL_Log("bones: %i", mesh->mNumBones);
        for (int j = 0; j < mesh->mNumBones; ++j)
        {
            SDL_Log("%s", mesh->mBones[j]->mName.C_Str());
        }
    }
    // LOGS END

    m_objectID = GameObjectID::ANIMATED_MODEL;

    m_objectName = mesh->mName.C_Str();
    m_directory = path.substr(0, path.find_last_of('/'));

    SDL_Log("model %s vertices %i, faces %i, bones %i", m_objectName.c_str(), mesh->mNumVertices, mesh->mNumFaces, mesh->mNumBones);

    if(!m_scene->HasAnimations()) Utils::error("mesh has bones, but m_scene has NOT animations");

    for(int i = 0; i < m_scene->mNumAnimations; ++i)
    {
        if(1 == m_scene->mNumAnimations)
        {
            m_currentAnimIndex = 0;
            break;
        }
        std::string animName = m_scene->mAnimations[i]->mName.C_Str();
        if(animName.find(m_objectName) != std::string::npos)
        {
            if(animName.find("run") != std::string::npos)
                m_animIDindexMap.insert(std::make_pair(AnimationID::RUN, i));
            else if(animName.find("stand") != std::string::npos)
                m_animIDindexMap.insert(std::make_pair(AnimationID::STAND, i));
            else if(animName.find("jump") != std::string::npos)
                m_animIDindexMap.insert(std::make_pair(AnimationID::JUMP, i));
            else if(animName.find("attack") != std::string::npos)
                m_animIDindexMap.insert(std::make_pair(AnimationID::ATTACK, i));
        }
    }

    if(m_animIDindexMap.empty() && -1 == m_currentAnimIndex) Utils::error("no animations for mesh ", m_objectName);
    m_currentAnimIndex = m_animIDindexMap.begin()->second;
    SDL_Log("m_currentAnimIndex %i", m_currentAnimIndex);

    m_globalInverseMatrix = m_scene->mRootNode->mTransformation;
    m_globalInverseMatrix.Inverse();

    // collect all data
    std::vector<Vertex> vertices{};
    std::vector<GLuint > indices{};
    std::vector<VertexBoneData> bonesIDweightsForVertex;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumVertices);
    bonesIDweightsForVertex.resize(mesh->mNumVertices);

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
        aiMaterial* material = m_scene->mMaterials[mesh->mMaterialIndex];

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

    // bones
    m_boneCount = mesh->mNumBones;
    m_boneMatrices.reserve(m_boneCount);

    for(int i = 0; i < m_boneCount; ++i)
    {
        std::string boneName = mesh->mBones[i]->mName.C_Str();

        std::map<std::string, int>::iterator iter = m_boneNameIndexMap.find(boneName);
        if(iter != m_boneNameIndexMap.end()) // bone exist in map
        {
            Utils::error("many bones have same name in one animated mesh");
        } else // allocate
        {
            BoneMatrix BM;
            m_boneMatrices.push_back(BM);
            m_boneMatrices.back().offsetMatrix = mesh->mBones[i]->mOffsetMatrix;
            m_boneNameIndexMap.insert(std::make_pair(boneName, i));
        }
        // collect all vertices to which bone has impact
        for(uint j = 0; j < mesh->mBones[i]->mNumWeights; ++j)
        {
            uint vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            //float weight = mesh->mBones[i]->mWeights[j].mWeight;

            bonesIDweightsForVertex.at(vertexID).addBoneData(i, mesh->mBones[i]->mWeights[j].mWeight);
        }
    }

    initGLBuffers(vertices, indices, bonesIDweightsForVertex);

    aiMatrix4x4 aiMatr{};
    Utils::findAiMatrixTransform(scene->mRootNode, mesh->mName, aiMatr);
    aiVector3D scaling{1.0f, 1.0f, 1.0f};
    aiQuaternion rotation{1.0f, 0.0f, 0.0f, 0.0f};
    aiVector3D position{0.0f, 0.0f, 0.0f};
    aiMatr.Decompose(scaling, rotation, position);
    m_posX = position.x;
    m_posY = position.y;
    m_posZ = position.z;

    m_modelMatrix = Utils::aiMatrToGlmMatr(aiMatr);

    //initCollisionObject(m_scene);

    ShaderProgramManager::instance()->makeShaderProgram(ProgramID::ANIMATED_MODEL,
                                                        "shaders/models/animModel.vert",
                                                        "shaders/models/animModel.frag");

    m_boneLocationInShader.resize(m_MAX_BONES);
    for(int i = 0; i < m_MAX_BONES; ++i)
    {
        std::string name = "boneTransforms[" + std::to_string(i) + "]"; // name like in shader
        m_boneLocationInShader.at(i) = glGetUniformLocation(
                ShaderProgramManager::instance()->getProgram(ProgramID::ANIMATED_MODEL), name.c_str());
    }

    m_MVP = Camera::instance()->getCamera3D();
}

AnimatedModel::~AnimatedModel()
{
    traceMethod();
    SDL_Log("~AnimatedModel()");

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBOvertices);
    glDeleteBuffers(1, &m_VBObones);
    glDeleteBuffers(1, &m_EBO);
}

void AnimatedModel::update()
{
    traceMethod();

    if(EventHandler::instance()->checkFingerEvent(FingerEventID::FINGER_FAST_TOUCH))
    {
        for(auto iter = m_animIDindexMap.begin(); iter != m_animIDindexMap.end(); ++iter)
        {
            if(m_currentAnimIndex == iter->second)
            {
                if(++iter != m_animIDindexMap.end())
                {
                    m_currentAnimIndex = iter->second;
                    break;
                } else
                {
                    m_currentAnimIndex = m_animIDindexMap.begin()->second;
                    break;
                }
            }
        }
    }
}

void AnimatedModel::draw()
{
    traceMethod();

    calculateTransforms();

    updateMVPmatrix();

    glUseProgram(ShaderProgramManager::instance()->getProgram(ProgramID::ANIMATED_MODEL));

    glUniformMatrix4fv(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::ANIMATED_MODEL), "MVP_matrix"),
                    1, GL_FALSE, glm::value_ptr(m_MVP));

    for(int i = 0; i < m_boneCount; ++i)
    {
        glUniformMatrix4fv(m_boneLocationInShader[i], 1, GL_TRUE, reinterpret_cast<GLfloat*>(&m_boneMatrices[i].finalWorldTransform));
    }

    for(auto iter = m_texturesMap.begin(); iter != m_texturesMap.end(); ++iter)
    {
        if(aiTextureType_DIFFUSE == iter->second)
        {
            // initialize sampler2D unit in shader (sampler2D = integer in shader)
            glUniform1i(glGetUniformLocation(ShaderProgramManager::instance()->getProgram(ProgramID::ANIMATED_MODEL),
                                             "material.diffuseTexture"), 0);
            glActiveTexture(GL_TEXTURE0); // then make this unit (sampler2D) active
            glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(iter->first)); // bind texture to this unit
        }
        else if(aiTextureType_SPECULAR == iter->second)
        {
            // initialize sampler2D unit in shader (sampler2D = integer in shader)
            glUniform1i(glGetUniformLocation(ShaderProgramManager::instance()->getProgram(ProgramID::ANIMATED_MODEL),
                                             "material.specularTexture"), 1);
            glActiveTexture(GL_TEXTURE1); // then make this unit (sampler2D) active
            glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(iter->first)); // bind texture to this unit
        }
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_LINES, m_indicesCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    for(int i = 0; i < m_texturesMap.size(); ++ i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    glUseProgram(0);
}

void AnimatedModel::playSound()
{
    traceMethod();

}

bool AnimatedModel::initGLBuffers(const std::vector<Vertex>& vertices,
                                const std::vector<GLuint>& indices,
                                const std::vector<VertexBoneData>& boneData)
{
    traceMethod();

    m_indicesCount = indices.size();

    // buffer data
    glGenBuffers(1, & m_VBOvertices);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOvertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // indices
    glGenBuffers(1, & m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indicesCount, &indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // bones data
    glGenBuffers(1, & m_VBObones);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBObones);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData)* boneData.size(), &boneData[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOvertices);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textCoords));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // bones
    glBindBuffer(GL_ARRAY_BUFFER, m_VBObones);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (void*)0); // INT pointer
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, boneWeights));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBindVertexArray(0);

    return true;
}

void AnimatedModel::updateMVPmatrix()
{
    traceMethod();

    if(m_hasCollisionObject)
    {

    } else
    {
        m_MVP = Camera::instance()->getCamera3D() * m_modelMatrix;
    }
}

void AnimatedModel::calculateTransforms()
{
    traceMethod();

    float timeInSec = static_cast<float>(SDL_GetTicks())/ 1000.0f;
    float tickPerSecond = static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mTicksPerSecond);
    float timeInTicks = timeInSec * ((0.0f == tickPerSecond) ? 24 : tickPerSecond);
    float animTime = fmod(timeInTicks, static_cast<float>(m_scene->mAnimations[m_currentAnimIndex]->mDuration));

    aiMatrix4x4 identity;
    readNodeHierarchy(animTime, m_scene->mRootNode, identity);
}

void AnimatedModel::readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4 parentTransform)
{
    traceMethod();

    const aiNodeAnim* nodeAnim = findNodeAnim(m_scene->mAnimations[m_currentAnimIndex], node->mName);

    aiMatrix4x4 nodeTransform = node->mTransformation;

    if(nodeAnim)
    {
        if(nodeAnim->mNumScalingKeys != nodeAnim->mNumRotationKeys) Utils::error("mNumScalingKeys != mNumRotationKeys");
        if(nodeAnim->mNumScalingKeys != nodeAnim->mNumPositionKeys) Utils::error("mNumScalingKeys != mNumPositionKeys");
        if(nodeAnim->mNumPositionKeys != nodeAnim->mNumRotationKeys) Utils::error("mNumPositionKeys != mNumRotationKeys");
        int currentFrameIndex{};
        for(int i = 0; i < nodeAnim->mNumPositionKeys - 1; ++i) // will use i + 1
        {
            if(animationTime < nodeAnim->mPositionKeys[i + 1].mTime) // less than next = actual frame did not finish
            {
                currentFrameIndex = i;
                break;
            }
        }

        aiMatrix4x4 scalingMatr = interpolateScaling(animationTime, nodeAnim, currentFrameIndex);

        aiMatrix4x4 rotationMatr = interpolateRotation(animationTime, nodeAnim, currentFrameIndex);

        aiMatrix4x4 translationMatr = interpolatePosition(animationTime, nodeAnim, currentFrameIndex);

        nodeTransform = translationMatr * rotationMatr * scalingMatr;
    }

    aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

    // node_name = bone_name = animation->chanel->node_name(nodeAnim contains node_name of affected node)
    std::string nodeName(node->mName.C_Str());
    std::map<std::string, int>::iterator iter = m_boneNameIndexMap.find(nodeName);
    if(iter != m_boneNameIndexMap.end())
    {
        int boneIndex = iter->second;
        m_boneMatrices[boneIndex].finalWorldTransform = m_globalInverseMatrix * globalTransform * m_boneMatrices[boneIndex].offsetMatrix;
    }

    for(int i = 0; i < node->mNumChildren; ++i)
    {
        readNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
    }
}

const aiNodeAnim* AnimatedModel::findNodeAnim(const aiAnimation* animation, const aiString nodeName)
{
    traceMethod();

    // channel in animation it is aiNodeAnim (aiNodeAnim its transformation for node, bone with same name)
    // numChannels == numBones
    for(int i = 0; i < animation->mNumChannels; ++i)
    {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if(nodeAnim->mNodeName == nodeName)
        {
            return nodeAnim;
        }
    }
    return nullptr;
}

aiMatrix4x4 AnimatedModel::interpolatePosition(const float animationTime, const aiNodeAnim* nodeAnim, const int currentFrameIndex)
{
    traceMethod();

    aiMatrix4x4 posMatr;

    if(1 == nodeAnim->mNumPositionKeys)
    {
        aiMatrix4x4::Translation(nodeAnim->mPositionKeys[0].mValue, posMatr);
        return posMatr;
    }

    int nextFrameIndex = currentFrameIndex + 1;
    assert(nextFrameIndex < nodeAnim->mNumPositionKeys);
    if(nodeAnim->mPositionKeys[currentFrameIndex].mValue == nodeAnim->mPositionKeys[nextFrameIndex].mValue)
    {// dont need interpolate
        aiMatrix4x4::Translation(nodeAnim->mPositionKeys[currentFrameIndex].mValue, posMatr);
        return posMatr;
    }

    float deltaTime = static_cast<float>(nodeAnim->mPositionKeys[nextFrameIndex].mTime) -
                      static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime);
    // factor = how much time passed from begin of current frame. represented in range 1 - 0
    float factor = (animationTime - static_cast<float>(nodeAnim->mPositionKeys[currentFrameIndex].mTime)) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    aiVector3D start = nodeAnim->mPositionKeys[currentFrameIndex].mValue;
    aiVector3D end = nodeAnim->mPositionKeys[nextFrameIndex].mValue;
    aiVector3D deltaVector = end - start;

    aiMatrix4x4::Translation(start + deltaVector * factor, posMatr);
    return posMatr;
}

aiMatrix4x4 AnimatedModel::interpolateRotation(const float animationTime, const aiNodeAnim* nodeAnim, const int currentFrameIndex)
{
    traceMethod();

    if(1 == nodeAnim->mNumRotationKeys)
        return aiMatrix4x4(nodeAnim->mRotationKeys[0].mValue.GetMatrix());

    int nextFrameIndex = currentFrameIndex + 1;
    assert(nextFrameIndex < nodeAnim->mNumRotationKeys);

    float deltaTime = static_cast<float>(nodeAnim->mRotationKeys[nextFrameIndex].mTime) -
                      static_cast<float>(nodeAnim->mRotationKeys[currentFrameIndex].mTime);
    // factor = how much time passed from begin of current frame. represented in range 1 - 0
    float factor = (animationTime - static_cast<float>(nodeAnim->mRotationKeys[currentFrameIndex].mTime)) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    aiQuaternion start = nodeAnim->mRotationKeys[currentFrameIndex].mValue;
    aiQuaternion end = nodeAnim->mRotationKeys[nextFrameIndex].mValue;
    aiQuaternion currentQuat = nlerp(start, end, factor);

    return aiMatrix4x4(currentQuat.GetMatrix());
}

aiMatrix4x4 AnimatedModel::interpolateScaling(const float animationTime, const aiNodeAnim* nodeAnim, const int currentFrameIndex)
{
    traceMethod();

    aiMatrix4x4 scalMatr;

    if(1 == nodeAnim->mNumScalingKeys)
    {
        aiMatrix4x4::Scaling(nodeAnim->mScalingKeys[0].mValue, scalMatr);
        return scalMatr;
    }

    int nextFrameIndex = currentFrameIndex + 1;
    assert(nextFrameIndex < nodeAnim->mNumScalingKeys);
    if(nodeAnim->mScalingKeys[currentFrameIndex].mValue == nodeAnim->mScalingKeys[nextFrameIndex].mValue)
    {// dont need interpolate
        aiMatrix4x4::Scaling(nodeAnim->mScalingKeys[currentFrameIndex].mValue, scalMatr);
        return scalMatr;
    }

    float deltaTime = static_cast<float>(nodeAnim->mScalingKeys[nextFrameIndex].mTime) -
                      static_cast<float>(nodeAnim->mScalingKeys[currentFrameIndex].mTime);
    // factor = how much time passed from begin of current frame. represented in range 1 - 0
    float factor = (animationTime - static_cast<float>(nodeAnim->mScalingKeys[currentFrameIndex].mTime)) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    aiVector3D start = nodeAnim->mScalingKeys[currentFrameIndex].mValue;
    aiVector3D end = nodeAnim->mScalingKeys[nextFrameIndex].mValue;
    aiVector3D deltaVector = end - start;

    aiMatrix4x4::Scaling(start + deltaVector * factor, scalMatr);
    return scalMatr;
}

aiQuaternion AnimatedModel::nlerp(aiQuaternion a, aiQuaternion b, float blend)
{
    //cout << a.w + a.x + a.y + a.z << endl;
    a.Normalize();
    b.Normalize();

    aiQuaternion result;
    float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    float one_minus_blend = 1.0f - blend;

    if (dot_product < 0.0f)
    {
        result.x = a.x * one_minus_blend + blend * -b.x;
        result.y = a.y * one_minus_blend + blend * -b.y;
        result.z = a.z * one_minus_blend + blend * -b.z;
        result.w = a.w * one_minus_blend + blend * -b.w;
    }
    else
    {
        result.x = a.x * one_minus_blend + blend * b.x;
        result.y = a.y * one_minus_blend + blend * b.y;
        result.z = a.z * one_minus_blend + blend * b.z;
        result.w = a.w * one_minus_blend + blend * b.w;
    }

    return result.Normalize();
}

void AnimatedModel::setAnimationToPlay(AnimationID id)
{
    traceMethod();

    std::map<AnimationID, int>::iterator iter = m_animIDindexMap.find(id);
    if(iter != m_animIDindexMap.end()) m_currentAnimIndex = iter->second;
    else if(!m_animIDindexMap.empty()) m_currentAnimIndex = m_animIDindexMap.begin()->second;
    else Utils::error("animations map is empty. can not set any animation");
}