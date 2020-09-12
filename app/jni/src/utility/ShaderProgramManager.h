#ifndef ENGINE1_SHADERPROGRAMMANAGER_H
#define ENGINE1_SHADERPROGRAMMANAGER_H

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <map>
#include "logs.h"

enum class ProgramID
{
    BUTTON, MODEL, GROUND, TEXT, ANIMATED_MODEL,

    AXIS // TODO delete
};

class ShaderProgramManager
{
public:

    bool makeShaderProgram(ProgramID id,
                             const char* const vertexShaderPath,
                             const char* const fragmentShaderPath);

    GLuint getProgram(ProgramID id)
    {
        traceMethod();

        std::map<ProgramID, GLuint>::iterator iter = m_shaderMap.find(id);
        if(iter != m_shaderMap.end()) return iter->second;
        else return 0;
    }

    void deleteFromShaderMap(ProgramID id)
    {
        traceMethod();

        std::map<ProgramID, GLuint>::iterator iter = m_shaderMap.find(id);
        if(iter != m_shaderMap.end())
        {
            glDeleteProgram(iter->second);
            m_shaderMap.erase(iter);
        }
    }

    static ShaderProgramManager* instance()
    {
        traceMethod();

        static ShaderProgramManager inst;
        return &inst;
    }

private:
    ShaderProgramManager();
    ~ShaderProgramManager();

    std::map<ProgramID, GLuint> m_shaderMap;
};


#endif //ENGINE1_SHADERPROGRAMMANAGER_H
