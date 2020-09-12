#ifndef ENGINE1_CAMERA_H
#define ENGINE1_CAMERA_H

#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../Game.h"

class Camera
{
public:

    glm::mat4 getCamera2D()
    {
        traceMethod();

        // use these Z plans for all 2D objects:
        // near plan: Z = 1.0f
        // far plan: Z = 0.0f (NOT -100.0 !!!!!!!)
        return glm::ortho(0.0f, static_cast<float>(Game::instance()->getScreenWidth()),
                          0.0f, static_cast<float>(Game::instance()->getScreenHeight()),
                          -1.1f, 100.0f);
    }

    glm::mat4 getCamera3D()
    {
        traceMethod();

        return getPerspective3D() * getVeiw3D();
    }

    glm::mat4 getVeiw3D()
    {
        traceMethod();

        updateCameraVectors();

        return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDirection, m_cameraUp);
    }

    glm::mat4 getPerspective3D()
    {
        traceMethod();

        return glm::perspectiveFov(glm::radians(45.0f),
                                   static_cast<float>(Game::instance()->getScreenWidth()),
                                   static_cast<float>(Game::instance()->getScreenHeight()),
                                   7.0f, 500.0f);
    }

    void setCameraPos(glm::vec3 pos) { traceMethod(); m_cameraPos = pos;}
    void setCameraFront(glm::vec3 front) { traceMethod(); m_cameraFrontPos = front;}

    static Camera* instance()
    {
        traceMethod();

        static Camera inst;
        return &inst;
    }

private:
    Camera(){}
    ~Camera(){}

    glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_cameraFrontPos = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_cameraUp;

    glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 m_cameraDirection;
    glm::vec3 m_cameraRight;

    void updateCameraVectors()
    {
        traceMethod();

        m_cameraDirection = glm::normalize(m_cameraFrontPos - m_cameraPos);
        m_cameraRight = glm::normalize(glm::cross(m_cameraDirection, m_worldUp));
        m_cameraUp = glm::normalize(glm::cross(m_cameraRight, m_cameraDirection));
    }
};

#endif //ENGINE1_CAMERA_H
