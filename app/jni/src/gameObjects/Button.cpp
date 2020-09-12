#include "Button.h"
#include "TextureManager.h"
#include "ShaderProgramManager.h"
#include "Camera.h"
#include "EventHandler.h"
#include "gtc/type_ptr.hpp"

Button::Button(float p_x, float p_y, float p_z, float width, float height,
               std::string imagePath1, std::string imagePath2)
{
    traceMethod();
    SDL_Log("Button()");

    m_objectID = GameObjectID::BUTTON;

    m_posX = p_x;
    m_posY = p_y;
    m_width = width;
    m_height = height;

    float vertices[]
            {   // positions                                 texture coords
                    p_x,          p_y,          p_z,                0.0f, 0.0f,
                    p_x + width,  p_y,          p_z,                1.0f, 0.0f,
                    p_x,          p_y + height, p_z,                0.0f, 1.0f,

                    p_x + width,  p_y,          p_z,                1.0f, 0.0f,
                    p_x,          p_y + height, p_z,                0.0f, 1.0f,
                    p_x + width,  p_y + height, p_z,                1.0f, 1.0f
            };

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(0); // enable for call glVertexAttribPointer(0, .....
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ShaderProgramManager::instance()->makeShaderProgram(ProgramID::BUTTON,
                                                        "shaders/button/button.vert",
                                                        "shaders/button/button.frag");

    m_textureID1 = imagePath1;
    if("" != imagePath2) m_textureID2 = imagePath2;
    else m_textureID2 = imagePath1;

    TextureManager::instance()->load_PNG_or_JPEG(m_textureID1);
    TextureManager::instance()->load_PNG_or_JPEG(m_textureID2);

    m_P = Camera::instance()->getCamera2D();
}

Button::~Button()
{
    traceMethod();
    SDL_Log("~Button()");

    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);

    TextureManager::instance()->eraseFromTextureMap(m_textureID1);
    TextureManager::instance()->eraseFromTextureMap(m_textureID2);
}

void Button::update()
{
    traceMethod();

    if(EventHandler::instance()->checkFingerEvent(FINGER_DOWN))
    {
        glm::vec2 fingerPos = EventHandler::instance()->getFingerDownPos();

        if(fingerPos.x > m_posX && fingerPos.x < (m_posX + m_width) &&
            fingerPos.y > m_posY && fingerPos.y < (m_posY + m_height))
        {
            m_buttonTouched = true;
        }
    }

    if(EventHandler::instance()->checkFingerEvent(FINGER_UP))
    {
        glm::vec2 fingerPos = EventHandler::instance()->getFingerUpPos();

        if(fingerPos.x > m_posX && fingerPos.x < (m_posX + m_width) &&
           fingerPos.y > m_posY && fingerPos.y < (m_posY + m_height) &&
           m_buttonTouched)
        {
            m_buttonAction();
        }
        m_buttonTouched = false;
    }
}

void Button::draw()
{
    traceMethod();

    m_P = Camera::instance()->getCamera2D();

    glUseProgram(ShaderProgramManager::instance()->getProgram(ProgramID::BUTTON));
    glUniformMatrix4fv(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::BUTTON), "P_matrix"),
            1, GL_FALSE, glm::value_ptr(m_P));

    if(m_buttonTouched)
    {
        glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(m_textureID2));
    }else
    {
        glBindTexture(GL_TEXTURE_2D, TextureManager::instance()->getTexture(m_textureID1));
    }

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void Button::playSound()
{
    traceMethod();
}