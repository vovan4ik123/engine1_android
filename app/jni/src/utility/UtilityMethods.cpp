#include "UtilityMethods.h"
#include "SDL_rwops.h"
#include "logs.h"

namespace Utils
{
    char* readFileToBuffer(const char* filename, uint* size)
    {
        traceMethod();

        SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
        if (NULL == rw) return nullptr;

        uint res_size = SDL_RWsize(rw);
        char* res = new char[res_size + 1];

        uint  nb_read_total = 0, nb_read = 1;
        char* buf = res;
        while (nb_read_total < res_size && nb_read != 0) {
            nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
            nb_read_total += nb_read;
            buf += nb_read;
        }
        SDL_RWclose(rw);
        if (nb_read_total != res_size) {
            delete[] res;
            return nullptr;
        }

        res[nb_read_total] = '\0';
        if (nullptr != size)
            *size = nb_read_total;

        return res;
    }

    bool writeFileToInternalStorageAndroid(const char* filename, const char* buffer, uint size)
    {
        //char *base_path = SDL_GetPrefPath("My Company", "Game");

        std::string path = SDL_AndroidGetInternalStoragePath();
        path += "/";
        path += filename;
        SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "wb");

        if(NULL == rw) error("error write file to internal storage");

        uint nb_written_total = 0, nb_written = 1;
        while(nb_written_total < size && nb_written != 0)
        {
            nb_written = SDL_RWwrite(rw, buffer, 1, (size - nb_written_total));
            nb_written_total += nb_written;
            buffer += nb_written;
        }
        SDL_RWclose(rw);

        if(nb_written_total != size)
        {
            SDL_RWops *rw = SDL_RWFromFile(path.c_str(), "w");
            SDL_RWclose(rw);
            return false;
        }

        return true;
    }

    GameLevelID intToLevelID(int x)
    {
        traceMethod();

        if(x < int(GameLevelID::LEVEL_1)) return GameLevelID::LEVEL_1; // return first
        if(x >= int(GameLevelID::COUNT)) return GameLevelID(int(GameLevelID::COUNT) - 1); // return last

        return GameLevelID(x);
    }

    void findAiMatrixTransform(const aiNode* node, aiString meshName, aiMatrix4x4& matrToSet)
    {
        traceMethod();

        if(node->mName == meshName) // node with same name contains matrix with mesh transformations
        {
            SDL_Log("found transform matrix for %s", meshName.data);
            matrToSet = node->mTransformation;
            return;
        }

        for(int i = 0; i < node->mNumChildren; ++i)
        {
            findAiMatrixTransform(node->mChildren[i], meshName, matrToSet);
        }
    }

    glm::mat4 aiMatrToGlmMatr(aiMatrix4x4 aiMat)
    {
        traceMethod();

        glm::mat4 result;
        result[0].x = aiMat.a1; result[1].x = aiMat.a2; result[2].x = aiMat.a3; result[3].x = aiMat.a4;
        result[0].y = aiMat.b1; result[1].y = aiMat.b2; result[2].y = aiMat.b3; result[3].y = aiMat.b4;
        result[0].z = aiMat.c1; result[1].z = aiMat.c2; result[2].z = aiMat.c3; result[3].z = aiMat.c4;
        result[0].w = aiMat.d1; result[1].w = aiMat.d2; result[2].w = aiMat.d3; result[3].w = aiMat.d4;

        return result;
    }

    void error(const std::string& s)
    {
        traceMethod();

        throw std::runtime_error(s);
    }

    void error(const std::string& s1, const std::string& s2)
    {
        traceMethod();

        error(s1 + " " + s2);
    }

    void error(const std::string& s, const double d)
    {
        traceMethod();

        std::ostringstream os;
        os << s << ": " << d;
        error(os.str());
    }
}