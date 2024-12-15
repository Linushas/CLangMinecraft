#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include "math3d.h"

#define NR_OF_CHUNKS 100

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

typedef struct windowModel {
    SDL_Window *win;
    SDL_GLContext glContext;
    EventH *eh;
    Camera *cam;
    unsigned int shaderProgram;
    GLuint hudShaderProgram;
    GLuint blockTextureID;
} WindowModel;

void mainGameLoop();
void render(unsigned int shaderProgram, WindowModel *wm, Chunk chunks[]);
void getWindowEvents(WindowModel *wm, Camera *cam);
void toggleFullscreen(WindowModel *wm);
Camera setupCamera();

#endif