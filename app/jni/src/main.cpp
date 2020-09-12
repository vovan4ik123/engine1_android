//
// Created by v on 24.04.2020.
//
#include "SDL.h"
#include "Game.h"
#include "utility/logs.h"
#include "Physics.h"
#include "Text.h"

const int MAX_FPS = 30;
const int TIME_FOR_1_LOOP = 1000 / MAX_FPS; // milli sec

uint FPS{};
uint fTime{};

int main(int argc, char* argv[])
{
    traceMethod();

    Uint32 frameStart{};
    Uint32 frameTime{};

    try
    {
        Game::instance()->init(); // should set running = true;

        while(Game::instance()->isRunning())
        {
            Game::instance()->update();
            Game::instance()->draw();
            Game::instance()->playSound();

            if(SDL_GetTicks() % 500 > 420)
            {
                FPS = 1000 / frameTime;
                if(FPS > MAX_FPS) FPS = MAX_FPS;
                fTime = frameTime;
            }

            frameTime = SDL_GetTicks() - frameStart;
            //SDL_Log("frameTime = %i ", frameTime);
            if(frameTime < TIME_FOR_1_LOOP)
            {
                SDL_Delay(TIME_FOR_1_LOOP  -frameTime);
            } // loop ends here.

            //time loop calculating starts here
            frameStart = SDL_GetTicks();

            // need do before next iteration
            Game::instance()->updateEventHandler(); // can close game in any state
        }
    }
    catch (const std::runtime_error &e)
    {
        SDL_Log("runtime error in main(): %s", e.what());
        Game::instance()->endGame();
    }
    catch (const std::out_of_range &e)
    {
        SDL_Log("out_of_range error in main(): %s", e.what());
        Game::instance()->endGame();
    }
    catch (const std::bad_cast &e)
    {
        SDL_Log("bad_cast error in main(): %s", e.what());
        Game::instance()->endGame();
    }
    catch (...)
    {
        SDL_Log("... error in main()");
        Game::instance()->endGame();
    }

    SDL_Log("end main()");
    return 0;
}

/*
    long long size{};
    delete[] Utils::readFileToBuffer("images/image2.jpg", &size);
    SDL_Log( "image size = %d", size);

    delete[] Utils::readFileToBuffer("sounds/gren1.wav", &size);
    SDL_Log( "sound size = %d", size);

    delete[] Utils::readFileToBuffer("fonts/Roboto-italic.ttf", &size);
    SDL_Log( "font size = %d", size);

    GLuint shProgram = Utils::makeShaderProgram("shaders/test_shader.vert", "shaders/test_shader.frag");
    glUseProgram(0);
    glDeleteProgram(shProgram);
 */

/*
#include <string>

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "glm.hpp"

#include "sqlite3.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

int db_callback(void* action,    // Data provided in the 4th argument of sqlite3_exec()
                int argc,      // The number of columns in row
                char** argv,   // An array of strings representing fields in the row
                char** column_name )   // An array of strings representing column names
{
    SDL_Log( "action: %s", action);

    for(int i = 0; i < argc; ++i)
    {
        SDL_Log( "column_name: %s    value: %s", column_name[i], argv[i] ? argv[i] : "NULL" );
    }

    return 0;
}

unsigned char* file_read(const char* filename, long long* size = nullptr) {
    SDL_RWops *rw = SDL_RWFromFile(filename, "rb");
    if (rw == NULL) return NULL;

    long long  res_size = SDL_RWsize(rw);
    unsigned char* res = (unsigned char*)malloc(res_size + 1);

    long long  nb_read_total = 0, nb_read = 1;
    unsigned char* buf = res;
    while (nb_read_total < res_size && nb_read != 0) {
        nb_read = SDL_RWread(rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose(rw);
    if (nb_read_total != res_size) {
        free(res);
        return NULL;
    }

    res[nb_read_total] = '\0';
    if (size != nullptr)
        *size = nb_read_total;
    return res;
}

void showNodeName(aiNode* node)
{
    SDL_Log("node->mName.data : %s", node->mName.data );
    for (uint i = 0; i < node->mNumChildren; i++)
    {
        showNodeName(node->mChildren[i]);
    }
}

int main(int argc, char* argv[])
{


    //SCREEN
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
    {
        SDL_Log( "SDL_Init good" );

        SDL_DisplayMode dm;
        SDL_GetDesktopDisplayMode(0, &dm); // if resolution change in SDL, this function will return the previous native display mode !!!
        SDL_Log( "screen WIDTH = %d, screen HEIGHT = %d" , dm.w, dm.h );
    }
    else
    {
        SDL_Log( "SDL_Init error" );
    }

    //GRAPHICS
    //OpenGL_ES_3.0

    //PHYSICS
    glm::vec4 v1 = glm::vec4(3.7f, 2.4f, 4.5f, 1.0f);
    SDL_Log( "v1 = %f", v1.y);

    //IMAGES
    SDL_RWops* rw = SDL_RWFromFile("images/image2.jpg", "rb");
    if (rw != NULL)
    {
        SDL_RWclose(rw);
        SDL_Log("image loaded");

        unsigned char* res = file_read("images/image2.jpg");

        free(res);
    } else
    {
        SDL_Log( "image error");
    }

    //SOUND EFFECTS
    // MIX_DEFAULT_FREQUENCY = sample rate = frequency = speed playing
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4069) == -1)
        SDL_Log( "SDL mixer init error" );
    else SDL_Log( "SDL mixer good" );

    rw = SDL_RWFromFile("sounds/gren1.wav", "rb");
    if (rw != NULL)
    {
        SDL_Log( "sound loaded");
        SDL_Log( "SDL_RWsize(rw) = %d", SDL_RWsize(rw));
        Mix_Chunk* sound_gr = Mix_LoadWAV_RW(rw, 0);
        SDL_RWclose(rw);
        if (sound_gr == NULL)
        {
            SDL_Log("Error load wave file");
        } else
        {
            Mix_PlayChannel(-1, sound_gr, 3);
        }
    } else
    {
        SDL_Log( "sound error");
    }

    //SCREEN TEXT
    FT_Library ft_lib;
    FT_Face ft_face;
    FT_Error ft_error;
    rw = SDL_RWFromFile("fonts/Roboto-italic.ttf", "rb");
    if (rw != NULL)
    {
        SDL_RWclose(rw);
        SDL_Log( "font loaded");

        ft_error = FT_Init_FreeType(&ft_lib);
        if (ft_error) SDL_Log( "FT_Init_FreeType error" );
        else SDL_Log( "FT_Init_FreeType good" );

        long long res_size = 0;
        unsigned char* res = file_read("fonts/Roboto-italic.ttf", &res_size);
        SDL_Log( "font size = %d", res_size);

        ft_error = FT_New_Memory_Face(ft_lib, res, res_size, 0, &ft_face);
        free(res);
        if (ft_error)
        {
            SDL_Log( "FT_New_Memory_Face() error" );
        } else
        {
            SDL_Log( "FT_New_Memory_Face() good" );
            ft_error = FT_Set_Pixel_Sizes(ft_face, 0, 24);
            if (ft_error) SDL_Log( "FT_Set_Pixel_Sizes() error" );
            else SDL_Log( "FT_Set_Pixel_Sizes() good" );
        }
    } else
    {
        SDL_Log( "font error");
    }


    //DATABASE SQLite
    sqlite3* db_object = nullptr;
    int error_code = 0;
    char* error_msg = nullptr;
    //const char* db_path = "/data/data/org.libsdl.app/databases/qwe.sqlite3";
    std::string db_path (SDL_AndroidGetInternalStoragePath());
    db_path += "/test1_db.sqlite";

    SDL_Log( "db_path: %s", db_path.c_str() );
    //SDL_Log( "Internal: %s", SDL_AndroidGetInternalStoragePath() );
    //SDL_Log( "External: %s", SDL_AndroidGetExternalStoragePath() );
    const char* sql = "CREATE TABLE IF NOT EXISTS PLAYER("
                      "ID INTEGER PRIMARY KEY, "
                      "NAME                   TEXT NOT NULL,"
                      "LVL                    INT NOT NULL,"
                      "DAMAGE_MIN             INT NOT NULL,"
                      "DAMAGE_MAX             INT NOT NULL);";

    // OPEN DB or CREATE 0 = success
    error_code = sqlite3_open_v2(db_path.c_str(), &db_object, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if(error_code)
    {
        SDL_Log( "sqlite3_open() error_code: %d", error_code );
    }
    // OPEN table in DB or CREATE
    error_code = sqlite3_exec(db_object, sql, NULL, NULL, &error_msg);
    if(error_code)
    {
        SDL_Log( "OPEN table error_code: %d", error_code );
    }
    SDL_Log( "sqlite3 error_msg: %s", error_msg );

    sqlite3_exec(db_object, "INSERT INTO PLAYER VALUES(1, 'db_first_row', 0, 0, 0);", NULL, NULL, &error_msg);

    //SDL_Log( "DELETE FROM PLAYER");
    //sqlite3_exec(db_object, "DELETE FROM PLAYER", NULL, NULL, &error_msg);

    // standard show all DB operation
    SDL_Log( "DB after creation");
    sqlite3_exec(db_object, "SELECT * FROM PLAYER", db_callback, NULL, &error_msg);


    //A column declared INTEGER PRIMARY KEY will autoincrement if we pass NULL there
    sql = "INSERT INTO PLAYER VALUES(null, 'dissm', 52, 14, 18);"
          "INSERT INTO PLAYER VALUES(null, 'enemy', 32, 5, 11);"
          "INSERT INTO PLAYER VALUES(null, 'warrior', 60, 40, 55);"
          "INSERT INTO PLAYER VALUES(null, 'elite_ogr', 50, 25, 40);";
    sqlite3_exec(db_object, sql, NULL, NULL, &error_msg);
    SDL_Log( "sqlite3 error_msg: %s", error_msg );

    SDL_Log( "DB after inserting");
    sqlite3_exec(db_object, "SELECT * FROM PLAYER", db_callback, NULL, &error_msg);

    sqlite3_exec(db_object, "DELETE FROM PLAYER WHERE ID = 2", NULL, NULL, &error_msg);

    SDL_Log( "DB after delete");
    sqlite3_exec(db_object, "SELECT * FROM PLAYER", db_callback, NULL, &error_msg);

    SDL_Log( "selection");
    sqlite3_exec(db_object, "SELECT * FROM PLAYER WHERE ID >= 3", db_callback, (void*)"SELECTION", &error_msg);

    sqlite3_free(error_msg);
    sqlite3_close(db_object);

    // 3D MODELS
    rw = SDL_RWFromFile("models/enemy/model.dae", "rb");
    if (rw != NULL)
    {
        SDL_Log( "model.dae size = %f KB", static_cast<float>(SDL_RWsize(rw) / 1000.0f));
        SDL_RWclose(rw);
    } else
    {
        SDL_Log( "model.dae loading error");
    }

    Assimp::Importer importer;
    const aiScene* scene = nullptr;

    long long res_size{};
    unsigned char* res = file_read("models/enemy/model.dae", &res_size);
    scene = importer.ReadFileFromMemory(res, res_size, aiProcess_Triangulate | aiProcess_FlipUVs);
    if(scene)
    {
        SDL_Log( "scene != nullptr");

        SDL_Log("scene->HasAnimations() : %d", scene->HasAnimations() );
        SDL_Log("scene->mNumMeshes : %d", scene->mNumMeshes );
        SDL_Log("scene->mAnimations[0]->mNumChannels : %d", scene->mAnimations[0]->mNumChannels );
        SDL_Log("scene->mAnimations[0]->mDuration : %f", scene->mAnimations[0]->mDuration );
        SDL_Log("scene->mAnimations[0]->mTicksPerSecond : %f", scene->mAnimations[0]->mTicksPerSecond );

        SDL_Log("Nodes names:");
        showNodeName(scene->mRootNode);

        for (uint i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* ai_mesh = scene->mMeshes[i];
            SDL_Log("ai_mesh->mNumBones : %d", ai_mesh->mNumBones );
            SDL_Log("ai_mesh->mNumVertices : %d", ai_mesh->mNumVertices );
            SDL_Log("ai_mesh->mNumFaces : %d", ai_mesh->mNumFaces );

            SDL_Log("Bones names:");
            for (uint i = 0; i < ai_mesh->mNumBones; ++i)
            {
                SDL_Log("%s", ai_mesh->mBones[i]->mName.data);
            }
        }
    } else
    {
        SDL_Log( "scene ====== nullptr");
    }
    importer.FreeScene();
    scene = nullptr;

    SDL_Log("HELLO !!!");

    SDL_Delay(5000);
    Mix_CloseAudio();
    return 0;
}
*/