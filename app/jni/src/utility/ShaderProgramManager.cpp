#include "ShaderProgramManager.h"
#include "UtilityMethods.h"
#include "SDL.h"

ShaderProgramManager::ShaderProgramManager()
{
        traceMethod();
}

ShaderProgramManager::~ShaderProgramManager()
{
        traceMethod();

        for(auto iter = m_shaderMap.begin(); iter != m_shaderMap.end(); ++iter)
        {
                glDeleteProgram(iter->second);
        }
        m_shaderMap.clear();
}

bool ShaderProgramManager::makeShaderProgram(ProgramID id,
                                                const char* const vertexShaderPath,
                                                const char* const fragmentShaderPath)
{
        traceMethod();

        if(m_shaderMap.find(id) != m_shaderMap.end())
        {
            return true; // exist
        }

        const char* vertexShaderCode = Utils::readFileToBuffer(vertexShaderPath);
        //SDL_Log("vertexShaderCode = %s", vertexShaderCode);
        const char* fragmentShaderCode = Utils::readFileToBuffer(fragmentShaderPath);
        //SDL_Log("fragmentShaderCode = %s", fragmentShaderCode);

        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertexShaderID, 1, &vertexShaderCode, 0);
        delete[] vertexShaderCode;
        glShaderSource(fragmentShaderID, 1, &fragmentShaderCode, 0);
        delete[] fragmentShaderCode;

#define DEBUG

        glCompileShader(vertexShaderID);
#ifdef DEBUG
        GLint compiled;
        glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &compiled); // проверка того что скомпилировалось
        if (GL_FALSE == compiled) {
            GLsizei len;
            glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &len); // с - GL_INFO_LOG_LENGTH - вернет размер ошибки

            GLchar* log = new GLchar[len + 1]; // для журнала к котором будет ошибка
            glGetShaderInfoLog(vertexShaderID, len, &len, log); // заполнит журнал сообщением об ошибке
            SDL_Log("Vertex Shader failed: %s", log);
            delete[] log;
        }
#endif // DEBUG

        glCompileShader(fragmentShaderID);
#ifdef DEBUG
        glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &compiled); // проверка того что скомпилировалось
        if (GL_FALSE == compiled) {
            GLsizei len;
            glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &len); // с - GL_INFO_LOG_LENGTH - вернет размер ошибки

            GLchar* log = new GLchar[len + 1]; // для журнала к котором будет ошибка
            glGetShaderInfoLog(fragmentShaderID, len, &len, log); // заполнит журнал сообщением об ошибке
            SDL_Log("Fragment Shader failed: %s", log);
            delete[] log;
        }
#endif // DEBUG

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShaderID);
        glAttachShader(program, fragmentShaderID);

        glLinkProgram(program);
#ifdef DEBUG
        GLint linked;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked == GL_FALSE)
        {
            GLsizei len;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

            GLchar* log = new GLchar[len + 1];
            glGetProgramInfoLog(program, len, &len, log);
            SDL_Log("glLinkProgram() failed: %s", log);
            delete[] log;
        } else
        {
            SDL_Log("glLinkProgram() success");
        }
#endif // DEBUG

        glDetachShader(program, vertexShaderID);
        glDetachShader(program, fragmentShaderID);
        glDeleteShader(vertexShaderID);     // only mark for delete in future if was not detached !!!
        glDeleteShader(fragmentShaderID);   // Will be deleted during call glDeleteProgram(programID);
        // or delete now if was detached

        // Shader objects don't really do anything, so there's no point in tracking them at all.
        // Shader objects should exist for just long enough to successfully link a program object.
        // After which time, the shaders should be detached from the program and deleted.

        //glDeleteShader() frees the memory and invalidates the name associated with the shader object.
        // This command effectively undoes the effects of a call to glCreateShader.
        // If a shader object to be deleted is attached to a program object,
        // it will be flagged for deletion, but it will not be deleted
        // until it is no longer attached to any program object,
        // for any rendering context (i.e., it must be detached from wherever it was attached before it will be deleted).

        //glDetachShader() detaches the shader object from the program object.
        // This command can be used to undo the effect of the command glAttachShader().
        // If shader has already been flagged for deletion by a call to glDeleteShader()
        // and it is not attached to any other program object, it will be deleted after it has been detached.

        //glDeleteProgram() frees the memory and invalidates the name associated with the program object
        // This command effectively undoes the effects of a call to glCreateProgram().
        // If a program object is in use as part of current rendering state, it will be flagged for deletion,
        // but it will not be deleted until it is no longer part of current state for any rendering context.
        //----- If a program object to be deleted has shader objects attached to it,
        // those shader objects will be automatically detached but not deleted
        // unless they have already been flagged for deletion by a previous call to glDeleteShader().

        m_shaderMap.insert(std::make_pair(id, program));
        return true;
}