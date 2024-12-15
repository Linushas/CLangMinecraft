#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include "block.h"

Face newFace(float x, float y, float z, int faceID) {
    float size = BLOCK_SIZE;
    float sideTexCoordX = (3 * 16) / 512.0f;
    float sideTexCoordY = (1 * 16) / 256.0f;
    float sideTexCoordX2 = sideTexCoordX + (16 / 512.0f);
    float sideTexCoordY2 = sideTexCoordY - (16 / 256.0f);

    float topTexCoordX = (2 * 16) / 512.0f;
    float topTexCoordY = (0 * 16) / 256.0f;
    float topTexCoordX2 = topTexCoordX + (16 / 512.0f);
    float topTexCoordY2 = topTexCoordY + (16 / 256.0f);

    switch(faceID) {
        case FRONT:
        Face front = {
            .vertices = {
                { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f,      sideTexCoordX, sideTexCoordY2  },
                { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f,      sideTexCoordX2, sideTexCoordY2 },
                { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f,      sideTexCoordX2, sideTexCoordY  },
                { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f,      sideTexCoordX, sideTexCoordY   }
            },
            .indices = { 0, 1, 2, 2, 3, 0 }
        };
        return front;
        case BACK:
        Face back = {
            .vertices = {
                { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f,      sideTexCoordX, sideTexCoordY2  },
                { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f,      sideTexCoordX2, sideTexCoordY2 },
                { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f,      sideTexCoordX2, sideTexCoordY  },
                { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f,      sideTexCoordX, sideTexCoordY   }
            },
            .indices = { 0, 1, 2, 2, 3, 0 }
        };
        return back;
        case LEFT:
        Face left = {
            .vertices = {
                { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    -1.0f, 0.0f, 0.0f,      sideTexCoordX, sideTexCoordY2  },
                { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    -1.0f, 0.0f, 0.0f,      sideTexCoordX2, sideTexCoordY2 },
                { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    -1.0f, 0.0f, 0.0f,      sideTexCoordX2, sideTexCoordY  },
                { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    -1.0f, 0.0f, 0.0f,      sideTexCoordX, sideTexCoordY   }
            },
            .indices = { 0, 1, 2, 2, 3, 0 }
        };
        return left;
        case RIGHT:
        Face right = {
            .vertices = {
                { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    1.0f, 0.0f, 0.0f,      sideTexCoordX, sideTexCoordY2  },
                { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    1.0f, 0.0f, 0.0f,      sideTexCoordX2, sideTexCoordY2 },
                { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    1.0f, 0.0f, 0.0f,      sideTexCoordX2, sideTexCoordY  },
                { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    1.0f, 0.0f, 0.0f,      sideTexCoordX, sideTexCoordY   }
            },
            .indices = { 0, 1, 2, 2, 3, 0 }
        };
        return right;
        case TOP:
        Face top = {
            .vertices = {
                { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 1.0f, 0.0f,      topTexCoordX, topTexCoordY2  },
                { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 1.0f, 0.0f,      topTexCoordX2, topTexCoordY2 },
                { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 1.0f, 0.0f,      topTexCoordX2, topTexCoordY  },
                { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 1.0f, 0.0f,      topTexCoordX, topTexCoordY   }
            },
            .indices = { 0, 1, 2, 2, 3, 0 }
        };
        return top;
        case BOTTOM:
        Face bottom = {
            .vertices = {
                { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, -1.0f, 0.0f,      topTexCoordX, topTexCoordY2  },
                { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, -1.0f, 0.0f,      topTexCoordX2, topTexCoordY2 },
                { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, -1.0f, 0.0f,      topTexCoordX2, topTexCoordY  },
                { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, -1.0f, 0.0f,      topTexCoordX, topTexCoordY   }
            },
            .indices = { 0, 1, 2, 2, 3, 0 }
        };
        return bottom;
    }
}

Chunk newChunk(float xPos, float yPos, float zPos) {
    Chunk newChunk = {.x = xPos, .y = yPos, .z = zPos};

    Vertex *allVertices = (Vertex*)malloc(sizeof(Vertex) * CHUNK_VOL *24);
    uint16_t *allIndices = (uint16_t*)malloc(sizeof(uint16_t) * CHUNK_VOL *36);

    int vertexOffset = 0;
    int indexOffset = 0;

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                newChunk.blocks[x][y][z].type = GRASS;
            }
        }
    }

    newChunk.blocks[5][CHUNK_SIZE-1][5].type = AIR;
    newChunk.blocks[5][CHUNK_SIZE-1][4].type = AIR;
    newChunk.blocks[4][CHUNK_SIZE-1][5].type = AIR;

    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                int visibleFaces[6] = {1,1,1,1,1,1}; //front back left right top bottom

                if (z < CHUNK_SIZE - 1 && newChunk.blocks[x][y][z + 1].type != AIR)
                    visibleFaces[0] = 0; 
                if (z > 0 && newChunk.blocks[x][y][z - 1].type != AIR)
                    visibleFaces[1] = 0; 
                if (x > 0 && newChunk.blocks[x - 1][y][z].type != AIR)
                    visibleFaces[2] = 0;
                if (x < CHUNK_SIZE - 1 && newChunk.blocks[x + 1][y][z].type != AIR)
                    visibleFaces[3] = 0;
                if (y < CHUNK_SIZE - 1 && newChunk.blocks[x][y + 1][z].type != AIR)
                    visibleFaces[4] = 0; 
                if (y > 0 && newChunk.blocks[x][y - 1][z].type != AIR)
                    visibleFaces[5] = 0;

                for(int faceIndex = 0; faceIndex < 6; faceIndex++) {
                    if(visibleFaces[faceIndex] && newChunk.blocks[x][y][z].type != AIR) {
                        float blockX = xPos + x * BLOCK_SIZE;
                        float blockY = yPos + y * BLOCK_SIZE;
                        float blockZ = zPos + z * BLOCK_SIZE;
                        Face current = newFace(blockX, blockY, blockZ, faceIndex);

                        memcpy(&allVertices[vertexOffset], current.vertices, sizeof(Vertex) * 4);
                        for (int i = 0; i < 6; i++) {
                            allIndices[indexOffset + i] = current.indices[i] + vertexOffset;
                        }

                        vertexOffset += 4;
                        indexOffset += 6;
                    }
                }

            }
        }
    }

    glGenVertexArrays(1, &newChunk.VAO);
    glGenBuffers(1, &newChunk.VBO);
    glGenBuffers(1, &newChunk.EBO);

    glBindVertexArray(newChunk.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, newChunk.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexOffset, allVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newChunk.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indexOffset, allIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    newChunk.indiceCount = indexOffset;

    free(allVertices);
    free(allIndices);

    return newChunk;
}

void renderChunk(Chunk chunk, int mode) {
    glBindVertexArray(chunk.VAO);
    glDrawElements(mode, chunk.indiceCount, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}


