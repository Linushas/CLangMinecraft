#ifndef HUD_H
#define HUD_H

#include <GL/glew.h>
#include "math3d.h"

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
} WindowModel;

void orthographicMatrix(Mat4x4 *matrix, float left, float right, float bottom, float top, float near, float far);
void renderHUD(WindowModel *wm, GLuint hudShaderProgram);

#endif