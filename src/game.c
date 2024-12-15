#include "game.h"
#include "hud.h"

void mainGameLoop(WindowModel *wm) {
    Chunk *chunks = malloc(NR_OF_CHUNKS * sizeof(Chunk));
    for(int x = 0; x < sqrt(NR_OF_CHUNKS); x++) {
        for(int z = 0; z < sqrt(NR_OF_CHUNKS); z++) {
            chunks[(x*(int)sqrt(NR_OF_CHUNKS)) + z] = newChunk(-(CHUNK_SIZE*sqrt(NR_OF_CHUNKS)/2) + x*CHUNK_SIZE, 0.0f, -(CHUNK_SIZE*sqrt(NR_OF_CHUNKS)/2) + z*CHUNK_SIZE);
        }
    }

    while (wm->eh->running)
    {
        getWindowEvents(wm, wm->cam);
        wm->cam->viewVec3 = subtractVec3d(wm->cam->target, wm->cam->eye);
        wm->cam->rightVec3 = crossProduct(wm->cam->viewVec3, wm->cam->up);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wm->blockTextureID);

        setupMatrices(&wm->cam->model, &wm->cam->view, &wm->cam->projection, wm->shaderProgram, wm->cam->eye, wm->cam->target, wm->cam->up);
        createRotationMatrix(&wm->cam->model, wm->cam->angleX, wm->cam->angleY, wm->cam->angleZ);

        unsigned int modelLoc = glGetUniformLocation(wm->shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(wm->shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(wm->shaderProgram, "projection");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &wm->cam->model.m[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &wm->cam->view.m[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &wm->cam->projection.m[0][0]);

        
        render(wm->shaderProgram, wm, chunks);
        renderHUD(wm, wm->hudShaderProgram); 
        glUseProgram(wm->shaderProgram);
        SDL_GL_SwapWindow(wm->win);
    }

    for(int i = 0; i < NR_OF_CHUNKS; i++) {
        glDeleteVertexArrays(1, &chunks[i].VAO);
        glDeleteBuffers(1, &chunks[i].VBO);
        glDeleteBuffers(1, &chunks[i].EBO);
        glDeleteProgram(wm->shaderProgram);
    }
}

void render(unsigned int shaderProgram, WindowModel *wm, Chunk chunks[])
{
    glClearColor(0.65f, 0.75f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    for(int i = 0; i < NR_OF_CHUNKS; i++) {
        if(wm->eh->r)
            renderChunk(chunks[i], GL_TRIANGLES);
        else
            renderChunk(chunks[i], GL_LINES);
    }
    glBindVertexArray(0);
}

void getWindowEvents(WindowModel *wm, Camera *cam)
{
    wm->eh->mouse_dx = 0.0f, wm->eh->mouse_dy = 0.0f;
    while (SDL_PollEvent(&(wm->eh->event)))
    {
        switch (wm->eh->event.type)
        {
        case SDL_QUIT:
            wm->eh->running = 0;
            break;

        case SDL_KEYDOWN:
            if(wm->eh->event.key.keysym.sym == SDLK_F11) toggleFullscreen(wm);
            if(wm->eh->event.key.keysym.sym == SDLK_r) wm->eh->r = !wm->eh->r;
            if(wm->eh->event.key.keysym.sym == SDLK_w) wm->eh->w = 1;
            if(wm->eh->event.key.keysym.sym == SDLK_a) wm->eh->a = 1;
            if(wm->eh->event.key.keysym.sym == SDLK_s) wm->eh->s = 1;
            if(wm->eh->event.key.keysym.sym == SDLK_d) wm->eh->d = 1;
            if(wm->eh->event.key.keysym.sym == SDLK_SPACE) wm->eh->space = 1;
            if(wm->eh->event.key.keysym.sym == SDLK_LCTRL) wm->eh->ctrl = 1;
            break;
        case SDL_KEYUP:
            if(wm->eh->event.key.keysym.sym == SDLK_w) wm->eh->w = 0;
            if(wm->eh->event.key.keysym.sym == SDLK_a) wm->eh->a = 0;
            if(wm->eh->event.key.keysym.sym == SDLK_s) wm->eh->s = 0;
            if(wm->eh->event.key.keysym.sym == SDLK_d) wm->eh->d = 0;
            if(wm->eh->event.key.keysym.sym == SDLK_SPACE) wm->eh->space = 0;
            if(wm->eh->event.key.keysym.sym == SDLK_LCTRL) wm->eh->ctrl = 0;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (wm->eh->event.button.button == SDL_BUTTON_LEFT)

            break;
        case SDL_MOUSEBUTTONUP:
            if (wm->eh->event.button.button == SDL_BUTTON_LEFT)
                
            break;

        case SDL_MOUSEWHEEL:
            // if (wm->eh->event.wheel.y > 0) eye->z -= 0.2f; // Zoom in
            // else if (wm->eh->event.wheel.y < 0) eye->z += 0.2f; // Zoom out
            break;

        case SDL_MOUSEMOTION:
            wm->eh->mouse_dx = wm->eh->event.motion.xrel;
            wm->eh->mouse_dy = wm->eh->event.motion.yrel;
            break;
        }
    }

    float speed = 0.1f;

    if(wm->eh->w) {
        cam->eye.x += cam->viewVec3.x * speed;
        cam->eye.z += cam->viewVec3.z * speed;
        cam->target.x += cam->viewVec3.x * speed;
        cam->target.z += cam->viewVec3.z * speed;
    }
    if(wm->eh->a) {
        cam->eye.x -= cam->rightVec3.x * speed;
        cam->eye.z -= cam->rightVec3.z * speed;
        cam->target.x -= cam->rightVec3.x * speed;
        cam->target.z -= cam->rightVec3.z * speed;
    }
    if(wm->eh->s) {
        cam->eye.x -= cam->viewVec3.x * speed;
        cam->eye.z -= cam->viewVec3.z * speed;
        cam->target.x -= cam->viewVec3.x * speed;
        cam->target.z -= cam->viewVec3.z * speed;
    }
    if(wm->eh->d) {
        cam->eye.x += cam->rightVec3.x * speed;
        cam->eye.z += cam->rightVec3.z * speed;
        cam->target.x += cam->rightVec3.x * speed;
        cam->target.z += cam->rightVec3.z * speed;
    }
    if(wm->eh->ctrl) {
        cam->eye.x -= cam->up.x * speed;
        cam->eye.y -= cam->up.y * speed;
        cam->eye.z -= cam->up.z * speed;
        cam->target.x -= cam->up.x * speed;
        cam->target.y -= cam->up.y * speed;
        cam->target.z -= cam->up.z * speed;
    }
    if(wm->eh->space) {
        cam->eye.x += cam->up.x * speed;
        cam->eye.y += cam->up.y * speed;
        cam->eye.z += cam->up.z * speed;
        cam->target.x += cam->up.x * speed;
        cam->target.y += cam->up.y * speed;
        cam->target.z += cam->up.z * speed;
    }

    cam->yaw += wm->eh->mouse_dx * sensitivity;
    cam->pitch -= wm->eh->mouse_dy * sensitivity;
    Vertex direction;
    direction.x = cosf(cam->yaw) * cosf(cam->pitch);
    direction.y = sinf(cam->pitch);
    direction.z = sinf(cam->yaw) * cosf(cam->pitch);
    direction = normalize(direction);
    cam->target.x = cam->eye.x + direction.x;
    cam->target.y = cam->eye.y + direction.y;
    cam->target.z = cam->eye.z + direction.z;
}

void toggleFullscreen(WindowModel *wm)
{
    wm->eh->fullScreen = !wm->eh->fullScreen;

    if (wm->eh->fullScreen)
    {
        SDL_SetWindowFullscreen(wm->win, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(0);
    }
    else
    {
        SDL_SetWindowFullscreen(wm->win, 0);
        SDL_ShowCursor(1);
    }

    int width, height;
    SDL_GetWindowSize(wm->win, &width, &height);
    glViewport(0, 0, width, height);
}

Camera setupCamera() {
    Mat4x4 model = {0}, view = {0}, projection = {0};
    Vertex eye = {0.0f, 20.0f, 0.0f};
    Vertex target = {0.0f, 0.0f, -4.0f};
    Vertex up = {0.0f, 1.0f, 0.0f};
    float angleX = 0.0f, angleY = 0.0f, angleZ = 0.0f;

    Camera cam = {
        .model = model, .view = view, .projection = projection,
        .eye = eye, .target = target, .up = up, 
        .angleX = angleX, .angleY = angleY, .angleZ = angleZ
    };
    return cam;
}