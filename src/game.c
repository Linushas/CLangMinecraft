#include <stdio.h>
#include "game.h"
#include "hud.h"

void updateDebug(WindowModel *wm) {
    SDL_SetRenderDrawColor(wm->debug->rend, 255, 255, 255, 255);
    SDL_RenderClear(wm->debug->rend);

    wm->debug->font = TTF_OpenFont("font/Roboto/Roboto-Light.ttf", 12);
    SDL_Color black = {0, 0, 0, 255};

    char string[60];
    // block coords
    sprintf(string, "x: %-10.2f, y:%-10.2f, z:%-10.2f", wm->cam->eye.x, wm->cam->eye.y, wm->cam->eye.z);
    SDL_Surface *fontSurf = TTF_RenderText_Blended(wm->debug->font, string, black);
    SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(wm->debug->rend, fontSurf);

    SDL_Rect font_rect = {5, 20, fontSurf->w, fontSurf->h};
    SDL_FreeSurface(fontSurf);
    SDL_RenderCopy(wm->debug->rend, fontTexture, NULL, &font_rect);

    // chunk coords
    sprintf(string, "Chunk: x: %-10.2f, z: %-10.2f", wm->cam->eye.x / 16, wm->cam->eye.z / 16);
    fontSurf = TTF_RenderText_Blended(wm->debug->font, string, black);
    fontTexture = SDL_CreateTextureFromSurface(wm->debug->rend, fontSurf);

    SDL_Rect font_rect2 = {5, 40, fontSurf->w, fontSurf->h};
    SDL_FreeSurface(fontSurf);
    SDL_RenderCopy(wm->debug->rend, fontTexture, NULL, &font_rect2);

    // fps
    sprintf(string, "fps: %.2f", wm->currentFps);
    fontSurf = TTF_RenderText_Blended(wm->debug->font, string, black);
    fontTexture = SDL_CreateTextureFromSurface(wm->debug->rend, fontSurf);

    SDL_Rect font_rect3 = {5, 60, fontSurf->w, fontSurf->h};
    SDL_FreeSurface(fontSurf);
    SDL_RenderCopy(wm->debug->rend, fontTexture, NULL, &font_rect3);



    SDL_RenderPresent(wm->debug->rend);
    SDL_DestroyTexture(fontTexture);
}

void reloadChunks(int *playerChunkX, int *playerChunkZ, World *world, ChunkMesh *chunks) {
    world->chunkCount = 0;
    for(int x = 0; x < WORLD_SIZE; x++) {
        for(int z = 0; z < WORLD_SIZE; z++) {
            if(abs(world->chunks[x][z].x - *playerChunkX) < world->renderDistance && 
               abs(world->chunks[x][z].z - *playerChunkZ) < world->renderDistance) {
                world->chunkCount++;
                world->chunks[x][z].inRange = 1;
            }
            else {
                world->chunks[x][z].inRange = 0;
            }
        }
    }
    
    int chunkIndex = 0;
    chunks = realloc(chunks, world->chunkCount * sizeof(ChunkMesh));
    for(int x = 0; x < WORLD_SIZE; x++) {
        for(int z = 0; z < WORLD_SIZE; z++) {
            if(world->chunks[x][z].inRange) {
                chunks[chunkIndex] = newChunk((float)world->chunks[x][z].x*CHUNK_SIZE, 0.0f, (float)world->chunks[x][z].z*CHUNK_SIZE);
                chunkIndex++;
            }
        }
    }
}

void mainGameLoop(WindowModel *wm) {
    World world;
    int playerChunkX = (int)wm->cam->eye.x / 16;
    int playerChunkZ = (int)wm->cam->eye.z / 16;

    world.renderDistance = 8;
    for(int x = 0; x < WORLD_SIZE; x++) {
        for(int z = 0; z < WORLD_SIZE; z++) {
            world.chunks[x][z].x = x - WORLD_SIZE/2;
            world.chunks[x][z].z = z - WORLD_SIZE/2;
            world.chunks[x][z].inRange = 0;
        }
    }

    world.chunkCount = 0;
    for(int x = 0; x < WORLD_SIZE; x++) {
        for(int z = 0; z < WORLD_SIZE; z++) {
            if(abs(world.chunks[x][z].x - playerChunkX) < world.renderDistance && 
               abs(world.chunks[x][z].z - playerChunkZ) < world.renderDistance) {
                world.chunkCount++;
                world.chunks[x][z].inRange = 1;
            }
            else {
                world.chunks[x][z].inRange = 0;
            }
        }
    }
    printf("chunks loaded: %d\n", world.chunkCount);
    int chunkIndex = 0;
    ChunkMesh *chunks = malloc(world.chunkCount * sizeof(ChunkMesh));
    for(int x = 0; x < WORLD_SIZE; x++) {
        for(int z = 0; z < WORLD_SIZE; z++) {
            if(world.chunks[x][z].inRange) {
                chunks[chunkIndex] = newChunk((float)world.chunks[x][z].x*CHUNK_SIZE, 0.0f, (float)world.chunks[x][z].z*CHUNK_SIZE);
                chunkIndex++;
            }
        }
    }


    int oldChunkX = playerChunkX, oldChunkZ = playerChunkZ;

    uint64_t NOW = SDL_GetPerformanceCounter();
    uint64_t LAST = 0;
    double deltaTime = 0;
    float m_secondCounter;
    float m_tempFps;
    float fps;
    while (wm->eh->running)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();


        getWindowEvents(wm, wm->cam, deltaTime);
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


        oldChunkX = playerChunkX;
        oldChunkZ = playerChunkZ;
        playerChunkX = (int)wm->cam->eye.x / 16;
        playerChunkZ = (int)wm->cam->eye.z / 16;
        if(playerChunkX != oldChunkX || playerChunkZ != oldChunkZ) {
            for(int i = 0; i < world.chunkCount; i++) {
                glDeleteVertexArrays(1, &chunks[i].VAO);
                glDeleteBuffers(1, &chunks[i].VBO);
                glDeleteBuffers(1, &chunks[i].EBO);
                glDeleteProgram(wm->shaderProgram);
            }
            reloadChunks(&playerChunkX, &playerChunkZ, &world, chunks);
        }
        
    
        
        render(wm->shaderProgram, wm, chunks, world);
        SDL_GL_SwapWindow(wm->win);
        updateDebug(wm);



        deltaTime = (double)((NOW - LAST)*1000 / (double)SDL_GetPerformanceFrequency() );
        if (m_secondCounter <= 1000) {
            m_secondCounter += deltaTime;
            m_tempFps++;
        }
        else 
        {
            fps = m_tempFps;
            m_secondCounter = 0;
            m_tempFps = 0;
            wm->currentFps = fps;
        }
    }

    free(chunks);
}

void render(unsigned int shaderProgram, WindowModel *wm, ChunkMesh chunks[], World world)
{
    glClearColor(0.65f, 0.75f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    for(int i = 0; i < world.chunkCount; i++) {
        if(wm->eh->r)
            renderChunk(chunks[i], GL_TRIANGLES);
        else
            renderChunk(chunks[i], GL_LINES);
    }
    glBindVertexArray(0);
}

void getWindowEvents(WindowModel *wm, Camera *cam, double deltaTime)
{
    wm->eh->mouse_dx = 0.0f, wm->eh->mouse_dy = 0.0f;
    while (SDL_PollEvent(&(wm->eh->event)))
    {
        switch (wm->eh->event.type)
        {
        case SDL_QUIT:
            printf("Received SDL_QUIT event\n");
            wm->eh->running = 0;
            break;
        case SDL_WINDOWEVENT:
            if (wm->eh->event.window.event == SDL_WINDOWEVENT_CLOSE){
                if (SDL_GetWindowFromID(wm->eh->event.window.windowID) == wm->win){
                    SDL_DestroyWindow(wm->win);
                    wm->win = NULL;
                    SDL_DestroyWindow(wm->debug->win);
                    wm->debug->win = NULL;
                }
                else if (SDL_GetWindowFromID(wm->eh->event.window.windowID) == wm->debug->win){
                    SDL_DestroyWindow(wm->debug->win);
                    wm->debug->win = NULL;
                }
                if (wm->win == NULL && wm->debug->win == NULL){
                    wm->eh->running = 0;
                }
            }
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
            ////
            break;

        case SDL_MOUSEMOTION:
            wm->eh->mouse_dx = wm->eh->event.motion.xrel;
            wm->eh->mouse_dy = wm->eh->event.motion.yrel;
            break;
        }
    }

    float speed = 10.0f * (deltaTime / 1000.0);

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