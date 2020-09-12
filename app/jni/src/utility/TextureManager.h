#ifndef ENGINE1_TEXTUREMANAGER_H
#define ENGINE1_TEXTUREMANAGER_H

#include <map>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include "logs.h"

class TextureManager
{
public:

    // pass here ".png" or ".jpeg" extension
    bool load_PNG_or_JPEG(const std::string imagePath, int* w = nullptr, int* h = nullptr);

    GLuint getTexture(const std::string imagePath)
    {
        traceMethod();

        std::map<const std::string, GLuint>::iterator iter = m_texturesMap.find(imagePath);
        if(iter != m_texturesMap.end())
        {
            return iter->second;
        }
        else return 0;
    }

    void eraseFromTextureMap(const std::string imagePath)
    {
        traceMethod();

        std::map<const std::string, GLuint>::iterator iter = m_texturesMap.find(imagePath);
        if(iter != m_texturesMap.end())
        {
            glDeleteTextures(1, &iter->second);

            m_texturesMap.erase(iter);
        }
    }

    static TextureManager* instance()
    {
        traceMethod();

        static TextureManager inst;
        return &inst;
    }

private:
    TextureManager();
    ~TextureManager();

    std::map<const std::string, GLuint> m_texturesMap{};
};


#endif //ENGINE1_TEXTUREMANAGER_H
