#include <stdio.h>
#include "game.h"
#include "shader.h"
#include "block.h"
#include "math3d.h"
#include "hud.h"

int initializeWindow(WindowModel *wm);
int initializeDebugWindow(DebugWindow *debug);
GLuint loadBlockTexures();

int main(int argc, char *argv[])
{   
    WindowModel wm;
    if(!initializeWindow(&wm))
        return -1;

    DebugWindow debug;
    if(!initializeDebugWindow(&debug))
        return -1;
    wm.debug = &debug;

    EventH eh = {.running = 1, .fullScreen = 0, .r = 0, .w = 0, .a = 0, .s = 0, .d = 0, .space = 0, .ctrl = 0};
    Camera cam = setupCamera();
    wm.eh = &eh;
    wm.cam = &cam;
    wm.blockTextureID = loadBlockTexures();

    setupMatrices(&wm.cam->model, &wm.cam->view, &wm.cam->projection, wm.shaderProgram, wm.cam->eye, wm.cam->target, wm.cam->up);

    loadShaders(&wm.shaderProgram);
    loadHUDShaders(&wm.hudShaderProgram);
    glUseProgram(wm.shaderProgram);
    glUniform1i(glGetUniformLocation(wm.shaderProgram, "ourTexture"), 0);

    mainGameLoop(&wm);

    SDL_GL_DeleteContext(wm.glContext);
    SDL_DestroyWindow(wm.win);

    SDL_DestroyRenderer(debug.rend);
    SDL_DestroyWindow(debug.win);
    IMG_Quit();
    TTF_Quit();

    SDL_Quit();
    return 0;
}

int initializeWindow(WindowModel *wm)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 0;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Unable to initialize SDL_Image: %s\n", IMG_GetError());
        return 0;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        exit(1);
    }


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    wm->win = SDL_CreateWindow("CMinecraft",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SW, SH, SDL_WINDOW_OPENGL);

    wm->glContext = SDL_GL_CreateContext(wm->win);
    if (!wm->glContext)
    {
        printf("Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(wm->win);
        SDL_Quit();
        return 0;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("Failed to initialize GLEW\n");
        SDL_GL_DeleteContext(wm->glContext);
        SDL_DestroyWindow(wm->win);
        SDL_Quit();
        return 0;
    }

    // Enable V-Sync
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE); 
    glCullFace(GL_BACK);


    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    return 1;
}

int initializeDebugWindow(DebugWindow *debug)
{
    debug->win = SDL_CreateWindow("Debug",
                               10, 30,
                               200, 200, 0);

    debug->rend = SDL_CreateRenderer(debug->win, -1, SDL_RENDERER_ACCELERATED);
    if (!debug->rend) {
        printf("Could not create renderer: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

GLuint loadBlockTexures() {
    SDL_Surface *surface = IMG_Load("textures/blocks.png");
    if (surface == NULL) {
        printf("Unable to load image: %s\n", IMG_GetError());
        return 1;
    }
    int width = surface->w;
    int height = surface->h;
    int format = surface->format->BytesPerPixel;

    // Check if the image has an alpha channel
    GLenum internalFormat, dataFormat;
    if (format == 4) { // RGBA
        internalFormat = GL_RGBA;
        dataFormat = GL_RGBA;
    } else if (format == 3) { // RGB
        internalFormat = GL_RGB;
        dataFormat = GL_RGB;
    } else {
        printf("Unsupported pixel format\n");
        return 1;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_FreeSurface(surface);

    return textureID;
}