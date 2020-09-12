#ifndef ENGINE1_BUTTON_H
#define ENGINE1_BUTTON_H

#include "GameObject.h"
#include <functional>

class Button : public GameObject
{
public:
    Button(float p_x, float p_y, float p_z, float width, float height,
           std::string imagePath1, std::string imagePath2 = "");
    virtual ~Button();


    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

    void setButtonAction(std::function<void()> action) { traceMethod(); m_buttonAction = action; }

private:
    std::function<void()> m_buttonAction;

    GLuint m_VBO{};
    GLuint m_VAO{};

    glm::mat4 m_P{}; // only projection matrix for 2D

    std::string m_textureID1{};
    std::string m_textureID2{};

    float m_width{}, m_height{};

    bool m_buttonTouched = false;
};


#endif //ENGINE1_BUTTON_H
