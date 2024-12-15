#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <GL/glew.h>
#include "math3d.h"

#define WORLD_SIZE 128 // 128*128 chunks

typedef struct eventHandler {
    SDL_Event event;
    int running;
    int fullScreen;
    int r;
    int w,a,s,d;
    int space,ctrl;
    float mouse_dx, mouse_dy;
} EventH;

typedef struct camera {
    Mat4x4 model, view, projection;
    Vertex eye, target, up;
    float angleX, angleY, angleZ;
    Vertex viewVec3, rightVec3;
    float pitch, yaw;
} Camera;

typedef struct debugWindow {
    SDL_Window *win;
    SDL_Renderer *rend;
    TTF_Font *font;
    EventH *eh;
} DebugWindow;

typedef struct windowModel {
    SDL_Window *win;
    DebugWindow *debug;
    SDL_GLContext glContext;
    EventH *eh;
    Camera *cam;
    unsigned int shaderProgram;
    GLuint hudShaderProgram;
    GLuint blockTextureID;
    float currentFps;
} WindowModel;

typedef struct chunk {
    int x, z;
    int chunkData;
    int inRange;
} Chunk;

typedef struct world {
    Chunk chunks[WORLD_SIZE][WORLD_SIZE];
    ChunkMesh **visibleChunks;
    int chunkCount;
    int renderDistance;
} World;

void mainGameLoop();
void render(unsigned int shaderProgram, WindowModel *wm, ChunkMesh chunks[], World world);
void getWindowEvents(WindowModel *wm, Camera *cam, double deltaTime);
void toggleFullscreen(WindowModel *wm);
Camera setupCamera();

#endif