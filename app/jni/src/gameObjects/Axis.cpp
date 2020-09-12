#include "Axis.h"
#include "SDL.h"
#include "Camera.h"
#include "ShaderProgramManager.h"
#include "gtc/type_ptr.hpp"

Axis::Axis()
{
    traceMethod();

    float vertices4[84]
    {
        -1.0f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        5.0f, 0.0f, 0.0f,      0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 5.0f,      0.0f, 0.0f, 0.0f,

        // X
        5.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,
        6.0f, 0.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        5.0f, 0.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        6.0f, 0.0f, 1.0f,      0.0f, 0.0f, 0.0f,
        // Z
        -1.0f, 0.0f, 5.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 5.0f,      0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 5.0f,      0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 6.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 6.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 6.0f,      0.0f, 0.0f, 0.0f,
    };

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 84, vertices4, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ShaderProgramManager::instance()->makeShaderProgram(ProgramID::AXIS,
                                                        "shaders/axis.vert",
                                                        "shaders/axis.frag");

    m_MVP = Camera::instance()->getCamera3D();
}

Axis::~Axis()
{
    traceMethod();

    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Axis::update()
{
    traceMethod();
}

void Axis::draw()
{
    traceMethod();

    m_MVP = Camera::instance()->getCamera3D();

    glLineWidth(3);

    glUseProgram(ShaderProgramManager::instance()->getProgram(ProgramID::AXIS));
    glUniformMatrix4fv(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::AXIS), "MVP_matrix"),
                       1, GL_FALSE, glm::value_ptr(m_MVP));

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, 14);

    glBindVertexArray(0);
    glUseProgram(0);
}

void Axis::playSound()
{
    traceMethod();
}