#include <stdio.h>
#include <string.h>
#include <SDL2/SDL_ttf.h>
#include "hud.h"

void orthographicMatrix(Mat4x4 *matrix, float left, float right, float bottom, float top, float near, float far) {
    memset(matrix, 0, sizeof(Mat4x4));
    matrix->m[0][0] = 2.0f / (right - left);
    matrix->m[1][1] = 2.0f / (top - bottom);
    matrix->m[2][2] = -2.0f / (far - near);
    matrix->m[3][3] = 1.0f;

    matrix->m[3][0] = -(right + left) / (right - left);
    matrix->m[3][1] = -(top + bottom) / (top - bottom);
    matrix->m[3][2] = -(far + near) / (far - near);
}

void renderHUD(WindowModel *wm, GLuint hudShaderProgram) {
    glUseProgram(hudShaderProgram);

    int width, height;
    SDL_GetWindowSize(wm->win, &width, &height);

    // Set up orthographic projection matrix
    Mat4x4 ortho;
    orthographicMatrix(&ortho, 0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(hudShaderProgram, "projection"), 1, GL_FALSE, &ortho.m[0][0]);

    // Disable depth testing for HUD rendering
    glDisable(GL_DEPTH_TEST);

    // Define rectangle vertices
    float rectVertices[] = {
        50.0f, 50.0f,
        250.0f, 50.0f,
        250.0f, 80.0f,
        50.0f, 80.0f,
    };

    // Use VAO/VBO for rectangle
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform4f(glGetUniformLocation(hudShaderProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f);

    // Draw the rectangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    // Re-enable depth testing
    glEnable(GL_DEPTH_TEST);
}