#include "TextureManager.h"
#include "SDL_rwops.h"
#include "SDL_image.h"

TextureManager::TextureManager()
{
    traceMethod();
}

TextureManager::~TextureManager()
{
    traceMethod();

    for(auto iter = m_texturesMap.begin(); iter != m_texturesMap.end(); ++iter)
    {
        glDeleteTextures(1, &iter->second);
    }

    m_texturesMap.clear();
}

bool TextureManager::load_PNG_or_JPEG(const std::string imagePath, int* w, int* h)
{
    traceMethod();

    if(m_texturesMap.find(imagePath) != m_texturesMap.end())
    {
        return true; // exist
    }

    SDL_RWops* rw = SDL_RWFromFile(imagePath.c_str(), "rb");
    if(NULL == rw) return false;

    SDL_Surface* surface = IMG_Load_RW(rw, 1);
    if(NULL == surface)
    {
        SDL_Log( "SDL image load Error: %s", IMG_GetError());
        return false;
    }

    if(nullptr != w) *w = surface->w;
    if(nullptr != h) *h = surface->h;

    GLuint glTextureID;
    glGenTextures(1, &glTextureID);
    glBindTexture(GL_TEXTURE_2D, glTextureID);

    int pixelFormat = GL_RGB;
    if(4 == surface->format->BytesPerPixel) pixelFormat = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, surface->w, surface->h, 0, pixelFormat, GL_UNSIGNED_BYTE, surface->pixels);

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_texturesMap.insert(std::make_pair(imagePath, glTextureID)); // add to map

    SDL_Log("loaded texture %s, index %i", imagePath.c_str(), glTextureID);
    return true;
}