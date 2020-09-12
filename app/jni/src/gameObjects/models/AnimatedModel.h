#ifndef ENGINE1_ANIMATEDMODEL_H
#define ENGINE1_ANIMATEDMODEL_H

#include "GameObject.h"

enum class AnimationID
{
    STAND, RUN, JUMP, ATTACK
};

struct VertexBoneData
{
    static const uint NUM_BONES_PER_VERTEX = 4; // one vertex can be affected maximum by 4 bones
    // init all values by -1 = information for shader that dont need look for matrix
    int boneIDs[NUM_BONES_PER_VERTEX] = {-1, -1, -1, -1};
    float boneWeights[NUM_BONES_PER_VERTEX] = {-1.0f, -1.0f, -1.0f, -1.0f};

    void addBoneData(int boneID, float weight)
    {
        traceMethod();

        for(int i = 0; i < NUM_BONES_PER_VERTEX; ++i)
        {
            if(-1.0f == boneWeights[i])
            {
                boneIDs[i] = boneID;
                boneWeights[i] = weight;
                return;
            }
        }
    }
};

// store loaded transform for bone and final transform after frame interpolation
struct BoneMatrix
{
    aiMatrix4x4 offsetMatrix{};
    aiMatrix4x4 finalWorldTransform{};
};

class AnimatedModel : public GameObject
{
public:
    AnimatedModel(const aiScene* m_scene, const aiMesh* mesh, std::string path);
    ~AnimatedModel();

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

    void setAnimationToPlay(AnimationID id);

private:
    // animation data start
    const aiScene* m_scene;
    aiMatrix4x4 m_globalInverseMatrix{};

    int m_currentAnimIndex = -1;
    std::map<AnimationID, int> m_animIDindexMap{};

    static const uint m_MAX_BONES = 20; // must be same in shader
    int m_boneCount{};
    std::map<std::string, int> m_boneNameIndexMap{};
    std::vector<BoneMatrix> m_boneMatrices;
    std::vector<int> m_boneLocationInShader{};

    void calculateTransforms();
    void readNodeHierarchy(const float animationTime, const aiNode* node, const aiMatrix4x4 parentTransform);
    const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const aiString nodeName);
    // count of mScalingKeys, mRotationKeys and mPositionKeys in animation should be same
    aiMatrix4x4 interpolatePosition(const float animationTime, const aiNodeAnim* nodeAnim, const int currentFrameIndex);
    aiMatrix4x4 interpolateRotation(const float animationTime, const aiNodeAnim* nodeAnim, const int currentFrameIndex);
    aiMatrix4x4 interpolateScaling(const float animationTime, const aiNodeAnim* nodeAnim, const int currentFrameIndex);
    aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend); // normalized linear interpolation

    // animation data end

    glm::mat4 m_MVP{};
    void updateMVPmatrix();

    bool initGLBuffers( const std::vector<Vertex>& vertices,
                        const std::vector<GLuint>& indices,
                        const std::vector<VertexBoneData>& boneData);
    GLuint m_VAO{};
    GLuint m_VBOvertices{};
    GLuint m_VBObones{};
    GLuint m_EBO{};
    GLuint m_indicesCount{};

    bool m_hasCollisionObject = false; // if true m_modelMatrix will be changed by bullet

    glm::vec3 m_collisObjectOrigOffset{0.0f, 0.0f, 0.0f};
};


#endif //ENGINE1_ANIMATEDMODEL_H
