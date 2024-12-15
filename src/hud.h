#ifndef HUD_H
#define HUD_H

#include <GL/glew.h>
#include "math3d.h"
#include "game.h"

void orthographicMatrix(Mat4x4 *matrix, float left, float right, float bottom, float top, float near, float far);
void renderHUD(WindowModel *wm, GLuint hudShaderProgram);

#endif