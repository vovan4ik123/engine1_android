#include "Text.h"
#include "Camera.h"
#include "UtilityMethods.h"
#include "ShaderProgramManager.h"
#include "gtc/type_ptr.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

Text::Text()
{
    traceMethod();

    SDL_Log("Text()");

    // copy fonts to internal storage
    uint res_size{};
    char* res = Utils::readFileToBuffer("fonts/Roboto.ttf", &res_size);
    if(!res) Utils::error("Roboto.ttf load error in Text.cpp");
    if(!Utils::writeFileToInternalStorageAndroid("Roboto.ttf", res, res_size))
        Utils::error("error write Roboto.ttf to internal storage in Text.cpp");
    delete[] res;
    res = nullptr;

    res = Utils::readFileToBuffer("fonts/Roboto_italic.ttf", &res_size);
    if(!res) Utils::error("Roboto_italic.ttf load error in Text.cpp");
    if(!Utils::writeFileToInternalStorageAndroid("Roboto_italic.ttf", res, res_size))
        Utils::error("error write Roboto_italic.ttf to internal storage in Text.cpp");
    delete[] res;
    res = nullptr;

    res = Utils::readFileToBuffer("fonts/Creamy_butter.ttf", &res_size);
    if(!res) Utils::error("Creamy_butter.ttf load error in Text.cpp");
    if(!Utils::writeFileToInternalStorageAndroid("Creamy_butter.ttf", res, res_size))
        Utils::error("error write Creamy_butter.ttf to internal storage in Text.cpp");
    delete[] res;
    res = nullptr;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Library ftLib;
    FT_Face ftFace;
    FT_Error ftError;
    std::string path{};

    // load font 1
    ftError = FT_Init_FreeType(&ftLib);
    if (ftError) Utils::error("freetype init font 1 error in Text.cpp");

    path = SDL_AndroidGetInternalStoragePath();
    path += "/Roboto.ttf";
    // DONT initialize freetype from memory !!!!!!!!!!!!!!!
    ftError = FT_New_Face(ftLib, path.c_str(), 0, &ftFace);
    if (ftError) Utils::error("face font 1 create error in Text.cpp");

    FT_Set_Pixel_Sizes(ftFace, 0, 64);

    for (int c = 32; c < 127; c++)
    {
        // load character glyph into face
        if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
            continue; // skip rest of code in loop and start next iteration

        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     ftFace->glyph->bitmap.width,
                     ftFace->glyph->bitmap.rows,
                     0, GL_RED,
                     GL_UNSIGNED_BYTE,
                     ftFace->glyph->bitmap.buffer);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
                texture,
                glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows), // size W H
                glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top), // bearing X Y
                static_cast<GLuint>(ftFace->glyph->advance.x)
        };

        m_robotoFontMap.insert(std::make_pair(c, character));
    }
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);

    // load font 2
    ftError = FT_Init_FreeType(&ftLib);
    if (ftError) Utils::error("freetype init font 2 error in Text.cpp");

    path = SDL_AndroidGetInternalStoragePath();
    path += "/Roboto_italic.ttf";
    // DONT initialize freetype from memory !!!!!!!!!!!!!!!
    ftError = FT_New_Face(ftLib, path.c_str(), 0, &ftFace);
    if (ftError) Utils::error("face font 2 create error in Text.cpp");

    FT_Set_Pixel_Sizes(ftFace, 0, 64);

    for (int c = 32; c < 127; c++)
    {
        // load character glyph into face
        if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
            continue; // skip rest of code in loop and start next iteration

        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     ftFace->glyph->bitmap.width,
                     ftFace->glyph->bitmap.rows,
                     0, GL_RED,
                     GL_UNSIGNED_BYTE,
                     ftFace->glyph->bitmap.buffer);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
                texture,
                glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows), // size W H
                glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top), // bearing X Y
                static_cast<GLuint>(ftFace->glyph->advance.x)
        };

        m_robotoItalicFontMap.insert(std::make_pair(c, character));
    }
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);

    // load font 3
    ftError = FT_Init_FreeType(&ftLib);
    if (ftError) Utils::error("freetype init font 3 error in Text.cpp");

    path = SDL_AndroidGetInternalStoragePath();
    path += "/Creamy_butter.ttf";
    // DONT initialize freetype from memory !!!!!!!!!!!!!!!
    ftError = FT_New_Face(ftLib, path.c_str(), 0, &ftFace);
    if (ftError) Utils::error("face font 3 create error in Text.cpp");

    FT_Set_Pixel_Sizes(ftFace, 0, 64);

    for (int c = 32; c < 127; c++)
    {
        // load character glyph into face
        if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER))
            continue; // skip rest of code in loop and start next iteration

        // generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     ftFace->glyph->bitmap.width,
                     ftFace->glyph->bitmap.rows,
                     0, GL_RED,
                     GL_UNSIGNED_BYTE,
                     ftFace->glyph->bitmap.buffer);

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
                texture,
                glm::ivec2(ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows), // size W H
                glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top), // bearing X Y
                static_cast<GLuint>(ftFace->glyph->advance.x)
        };

        m_creamyFontMap.insert(std::make_pair(c, character));
    }
    FT_Done_Face(ftFace);
    FT_Done_FreeType(ftLib);


    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // use standard 4 byte represents the colors of a texture

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ShaderProgramManager::instance()->makeShaderProgram(ProgramID::TEXT,
                                                        "shaders/text/text.vert",
                                                        "shaders/text/text.frag");

}

Text::~Text()
{
    traceMethod();
    SDL_Log("~Text()");

    for(auto iter = m_robotoFontMap.begin(); iter != m_robotoFontMap.end(); ++iter)
    {
        glDeleteTextures(1, &iter->second.textureID);
    }
    m_robotoFontMap.clear();

    for(auto iter = m_robotoItalicFontMap.begin(); iter != m_robotoItalicFontMap.end(); ++iter)
    {
        glDeleteTextures(1, &iter->second.textureID);
    }
    m_robotoItalicFontMap.clear();

    for(auto iter = m_creamyFontMap.begin(); iter != m_creamyFontMap.end(); ++iter)
    {
        glDeleteTextures(1, &iter->second.textureID);
    }
    m_creamyFontMap.clear();

    glDeleteBuffers(1, &m_VBO);
    glDeleteVertexArrays(1, &m_VAO);
}

void Text::draw2Dtext(FontID fontID, std::string text, glm::vec3 pos, glm::vec3 color, float scale, float transparency)
{
    traceMethod();

    m_P = Camera::instance()->getCamera2D();

    glUseProgram(ShaderProgramManager::instance()->getProgram(ProgramID::TEXT));
	
    glUniform3f(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::TEXT),"color"),
            color.x, color.y, color.z);
    glUniform1f(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::TEXT),"transparency"),
                transparency);
    glUniformMatrix4fv(glGetUniformLocation(
            ShaderProgramManager::instance()->getProgram(ProgramID::TEXT),"P_matrix"),
                       1, GL_FALSE, glm::value_ptr( (m_P)));
    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0);

    for (std::string::iterator textIter = text.begin(); textIter != text.end(); ++textIter)
    {
        Character character;
        std::map<char, Character>::iterator mapIter;

        if(FontID::ROBOTO == fontID)
        {
            mapIter = m_robotoFontMap.find(*textIter);
            if(m_robotoFontMap.end() != mapIter) character = mapIter->second;
        }
        else if(FontID::ROBOTO_ITALIC == fontID)
        {
            mapIter = m_robotoItalicFontMap.find(*textIter);
            if(m_robotoFontMap.end() != mapIter) character = mapIter->second;
        }
        else if(FontID::CREAMY == fontID)
        {
            mapIter = m_creamyFontMap.find(*textIter);
            if(m_robotoFontMap.end() != mapIter) character = mapIter->second;
        }

        float x_pos = pos.x + character.bearing.x * scale;
        float y_pos = pos.y - (character.size.y - character.bearing.y) * scale;

        float w = character.size.x * scale;
        float h = character.size.y * scale;
        float vertices[]
                {	// x			// y		// z	// texture
                        x_pos,		y_pos + h,  0.0f,	0.0, 0.0,
                        x_pos,		y_pos,		0.0f,	0.0, 1.0,
                        x_pos + w,	y_pos,		0.0f,	1.0, 1.0,

                        x_pos,		y_pos + h,  0.0f,	0.0, 0.0,
                        x_pos + w,	y_pos,		0.0f,	1.0, 1.0,
                        x_pos + w,	y_pos + h,  0.0f,	1.0, 0.0
                };

        glBindTexture(GL_TEXTURE_2D, character.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 30, vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        pos.x += (character.advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

}