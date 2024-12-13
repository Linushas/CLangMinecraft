#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include "block.h"

Block newBlock(float x, float y, float z) {
    float size = BLOCK_SIZE;
    Block newBlock = {
        .vertices = {
            // Front face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f },
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f },
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, 0.0f, 1.0f },

            // Back face
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, 0.0f, -1.0f },

            // Left face
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    -1.0f, 0.0f, 0.0f },

            // Right face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    1.0f, 0.0f, 0.0f },

            // Top face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 1.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.0f, 1.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 1.0f, 0.0f },
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.0f, 1.0f, 0.0f },

            // Bottom face
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, -1.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.0f, -1.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, -1.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.0f, -1.0f, 0.0f },
        },
        .indices = {
            // Front face
            0, 1, 2, 2, 3, 0,
            // Back face
            4, 5, 6, 6, 7, 4,
            // Left face
            8, 9, 10, 10, 11, 8,
            // Right face
            12, 13, 14, 14, 15, 12,
            // Top face
            16, 17, 18, 18, 19, 16,
            // Bottom face
            20, 21, 22, 22, 23, 20,
        }
    };

    return newBlock;
}

Chunk newChunk(float xPos, float yPos, float zPos) {
    Chunk newChunk = {.x = xPos, .y = yPos, .z = zPos};

    Vertex *allVertices = (Vertex*)malloc(sizeof(Vertex) * CHUNK_VOL *24);
    unsigned int *allIndices = (unsigned int*)malloc(sizeof(unsigned int) * CHUNK_VOL *36);

    unsigned int vertexOffset = 0;
    unsigned int indexOffset = 0; 

    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int y = 0; y < CHUNK_SIZE; y++) {
            for(int x = 0; x < CHUNK_SIZE; x++) {
                Block block = newBlock(x*BLOCK_SIZE, y*BLOCK_SIZE, z*BLOCK_SIZE);
                for(int i = 0; i < 24; i++) {
                    allVertices[vertexOffset + i] = block.vertices[i];
                }
                for(int i = 0; i < 36; i++) {
                    allIndices[indexOffset + i] = block.indices[i] + vertexOffset;
                }

                vertexOffset += 24;
                indexOffset += 36;
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexOffset, allIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    newChunk.indiceCount = indexOffset;

    free(allVertices);
    free(allIndices);

    return newChunk;
}

void renderChunk(Chunk chunk, int mode) {
    glBindVertexArray(chunk.VAO);
    glDrawElements(mode, chunk.indiceCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    // printf("%d\n", chunk.indiceCount);
}


