#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "hud.h"

RayHit raycast(World *world, Vertex eye, Vertex viewVec, float maxDistance) {
    RayHit hit = {0, 0, 0, -1, 0};
    
    float stepSize = 0.01f; // Adjust for precision and performance
    Vertex rayDir = normalize(viewVec);
    
    Vertex current = eye;
    for (float t = 0; t < maxDistance*10; t += stepSize) {
        int blockX = (int)floor(current.x);
        int blockY = (int)floor(current.y);
        int blockZ = (int)floor(current.z);
        
        // Ensure coordinates are within bounds
        if (blockX < 0 || blockX >= WORLD_SIZE * CHUNK_SIZE ||
            blockY < 0 || blockY >= CHUNK_SIZE ||
            blockZ < 0 || blockZ >= WORLD_SIZE * CHUNK_SIZE) {
            break;
        }

        // Determine chunk and local block coordinates
        int chunkX = blockX / CHUNK_SIZE;
        int chunkZ = blockZ / CHUNK_SIZE;
        int localX = blockX % CHUNK_SIZE;
        int localY = blockY;
        int localZ = blockZ % CHUNK_SIZE;

        // Check if block is not AIR
        if (world->chunks[chunkX][chunkZ].chunkData[localX][localY][localZ] != AIR) {
            hit.blockX = blockX;
            hit.blockY = blockY;
            hit.blockZ = blockZ;
            hit.hit = 1;

            // Determine face
            Vertex diff = subtractVec3d(current, (Vertex){blockX + 0.5f, blockY + 0.5f, blockZ + 0.5f});
            if (fabs(diff.x) > fabs(diff.y) && fabs(diff.x) > fabs(diff.z)) {
                hit.face = (diff.x > 0) ? 2 : 3; // Left or Right
            } else if (fabs(diff.y) > fabs(diff.x) && fabs(diff.y) > fabs(diff.z)) {
                hit.face = (diff.y > 0) ? 5 : 4; // Bottom or Top
            } else {
                hit.face = (diff.z > 0) ? 1 : 0; // Back or Front
            }

            break;
        }

        // Advance the ray
        current.x += rayDir.x * stepSize;
        current.y += rayDir.y * stepSize;
        current.z += rayDir.z * stepSize;
    }
    return hit;
}


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

    // looking at
    sprintf(string, "sel: x: %-10d, y:%-10d, z:%-10d", wm->rayHit->blockX, wm->rayHit->blockY, wm->rayHit->blockZ);
    fontSurf = TTF_RenderText_Blended(wm->debug->font, string, black);
    fontTexture = SDL_CreateTextureFromSurface(wm->debug->rend, fontSurf);

    SDL_Rect font_rect4 = {5, 80, fontSurf->w, fontSurf->h};
    SDL_FreeSurface(fontSurf);
    SDL_RenderCopy(wm->debug->rend, fontTexture, NULL, &font_rect4);


    SDL_RenderPresent(wm->debug->rend); 
    SDL_DestroyTexture(fontTexture);
}

void reloadChunks(int *playerChunkX, int *playerChunkZ, World *world, ChunkMesh *chunks, RayHit hit) {
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
                chunks[chunkIndex] = newChunk(world, (float)world->chunks[x][z].x*CHUNK_SIZE, 0.0f, (float)world->chunks[x][z].z*CHUNK_SIZE, x, z);
                chunkIndex++;
            }
        }
    }
}

void mainGameLoop(WindowModel *wm) {
    srand(time(NULL));

    World world;
    int playerChunkX = (int)wm->cam->eye.x / 16;
    int playerChunkZ = (int)wm->cam->eye.z / 16;


    world.chunks = (Chunk**) malloc(WORLD_SIZE * sizeof(Chunk*));
    if (!world.chunks) {
        printf("could not load world!\n");
        return;
    }
    for (int x = 0; x < WORLD_SIZE; x++) {
        world.chunks[x] = (Chunk*) malloc(WORLD_SIZE * sizeof(Chunk));
        if (!world.chunks[x]) {
            printf("could not load world!\n");
            return;
        }
        for (int z = 0; z < WORLD_SIZE; z++) {
            

            world.chunks[x][z].chunkData = (uint8_t***) malloc(CHUNK_SIZE * sizeof(uint8_t**));
            if (!world.chunks[x][z].chunkData) {
                printf("could not load world!\n");
                return;
            }
            for (int cx = 0; cx < CHUNK_SIZE; cx++) {
                world.chunks[x][z].chunkData[cx] = (uint8_t**) malloc(CHUNK_SIZE * sizeof(uint8_t*));
                if (!world.chunks[x][z].chunkData[cx]) {
                    printf("could not load world!\n");
                    return;
                }
                for (int cy = 0; cy < CHUNK_SIZE; cy++) {
                    world.chunks[x][z].chunkData[cx][cy] = (uint8_t*) malloc(CHUNK_SIZE * sizeof(uint8_t));
                    if (!world.chunks[x][z].chunkData[cx][cy]) {
                        printf("could not load world!\n");
                        return;
                    }
                }
            }


        }
    }
    // Initialize chunk data
    int height = 0, heightChunk = 0;
    for(int x = 0; x < WORLD_SIZE; x++) {
        for(int z = 0; z < WORLD_SIZE; z++) {
            if (world.chunks[x] && world.chunks[x][z].chunkData) {
                world.chunks[x][z].x = x - WORLD_SIZE/2;
                world.chunks[x][z].z = z - WORLD_SIZE/2;
                world.chunks[x][z].inRange = 0;
                heightChunk = rand()%8;
                if(heightChunk+z < 4) heightChunk = heightChunk + 5;
                for(int cx = 0; cx < CHUNK_SIZE; cx++) {
                    if(cx % 6 == 0)
                        if(heightChunk+z < 4)
                            heightChunk++;
                        else
                            heightChunk--;
                    for(int cz = 0; cz < CHUNK_SIZE; cz++) {
                        height = rand()%10;
                        if(height > 4) height = rand()%10;
                        if(height > 4) height = rand()%10;
                        if(height > 8) height = rand()%4;
                        if(height > 4) height = rand()%2;
                        if(height > 1) height = rand()%1;
                        int cy;
                        for(cy = 0; cy < (CHUNK_SIZE-11)+height + heightChunk; cy++) {
                            if (world.chunks[x][z].chunkData[cx] && world.chunks[x][z].chunkData[cx][cy]) {
                                world.chunks[x][z].chunkData[cx][cy][cz] = STONE;
                            }
                        }
                        for(cy; cy < (CHUNK_SIZE-10)+height + heightChunk; cy++) {
                            if (world.chunks[x][z].chunkData[cx] && world.chunks[x][z].chunkData[cx][cy]) {
                                world.chunks[x][z].chunkData[cx][cy][cz] = DIRT;
                            }
                        }
                        for(cy; cy < (CHUNK_SIZE-9)+height + heightChunk; cy++) {
                            if (world.chunks[x][z].chunkData[cx] && world.chunks[x][z].chunkData[cx][cy]) {
                                world.chunks[x][z].chunkData[cx][cy][cz] = GRASS;
                            }
                        }
                        for(cy; cy < CHUNK_SIZE; cy++) {
                            if (world.chunks[x][z].chunkData[cx] && world.chunks[x][z].chunkData[cx][cy]) {
                                world.chunks[x][z].chunkData[cx][cy][cz] = AIR;
                            }
                        }
                    }
                }
            }
        }
    }

    world.renderDistance = 8;

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
                chunks[chunkIndex] = newChunk(&world, (float)world.chunks[x][z].x*CHUNK_SIZE, 0.0f, (float)world.chunks[x][z].z*CHUNK_SIZE, x, z);
                chunkIndex++;
            }
        }
    }

    RayHit hit = raycast(&world, wm->cam->eye, wm->cam->viewVec3, 50.0f);
    wm->rayHit = &hit;

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
            }
            reloadChunks(&playerChunkX, &playerChunkZ, &world, chunks, hit);
        }


        
        hit = raycast(&world, wm->cam->eye, wm->cam->viewVec3, 50.0f);

        glClearColor(0.65f, 0.75f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(wm->shaderProgram);
        
        render(wm->shaderProgram, wm, chunks, world);
        glDisable(GL_DEPTH_TEST);
        selectedCube(hit);
        glEnable(GL_DEPTH_TEST);
        glBindVertexArray(0);
        
        SDL_GL_SwapWindow(wm->win);
        if(!wm->eh->r)
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

    // CLEAN UP
    for (int x = 0; x < WORLD_SIZE; x++) {
        for (int z = 0; z < WORLD_SIZE; z++) {
            for (int cx = 0; cx < CHUNK_SIZE; cx++) {
                for (int cy = 0; cy < CHUNK_SIZE; cy++) {
                    free(world.chunks[x][z].chunkData[cx][cy]);
                }
                free(world.chunks[x][z].chunkData[cx]);
            }
            free(world.chunks[x][z].chunkData);
        }
        free(world.chunks[x]);
    }
    free(world.chunks);
    free(chunks);
    glDeleteProgram(wm->shaderProgram);
}

void render(unsigned int shaderProgram, WindowModel *wm, ChunkMesh chunks[], World world)
{
    for(int i = 0; i < world.chunkCount; i++) {
        if(wm->eh->r)
            renderChunk(chunks[i], GL_TRIANGLES);
        else
            renderChunk(chunks[i], GL_LINES);
    }
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