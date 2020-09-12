#ifndef ENGINE1_AXIS_H
#define ENGINE1_AXIS_H

#include "GameObject.h"

class Axis : public GameObject
{
public:
    Axis();
    virtual ~Axis();

    virtual void update() override;
    virtual void draw() override;
    virtual void playSound() override;

private:
    GLuint m_VBO{};
    GLuint m_VAO{};

    glm::mat4 m_MVP{};

};


#endif //ENGINE1_AXIS_H
