#ifndef ENGINE1_TEXT_H
#define ENGINE1_TEXT_H


#include "glm.hpp"
#include <map>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <string>
#include "logs.h"

enum class FontID
{
    CREAMY, ROBOTO, ROBOTO_ITALIC
};

class Text
{
public:
    static Text* instance()
    {
        traceMethod();

        static Text inst;
        return &inst;
    }

    // transparency: 1 = visible, 0 = transparent
    void draw2Dtext(FontID fontID, std::string text, glm::vec3 pos, glm::vec3 color, float scale, float transparency);

private:
    Text();
    ~Text();

    struct Character
    {
        GLuint textureID{};
        glm::ivec2 size;
        glm::ivec2 bearing;
        uint advance;
    };


    glm::mat4 m_P; // matrix for 2D text
    GLuint m_VBO;
    GLuint m_VAO;

    std::map<char, Character> m_robotoFontMap{};
    std::map<char, Character> m_robotoItalicFontMap{};
    std::map<char, Character> m_creamyFontMap{};
};


#endif //ENGINE1_TEXT_H
