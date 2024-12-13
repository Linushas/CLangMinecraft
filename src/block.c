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
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, 1.0f },

            // Back face
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 0.0f, -1.0f },

            // Left face
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   -1.0f, 0.0f, 0.0f },

            // Right face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   1.0f, 0.0f, 0.0f },

            // Top face
            { x + 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },
            { x - 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },
            { x + 0.5f * size, y + 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, 1.0f, 0.0f },

            // Bottom face
            { x + 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z + 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
            { x - 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
            { x + 0.5f * size, y - 0.5f * size, z - 0.5f * size,    0.5f, 0.5f, 0.5f,   0.0f, -1.0f, 0.0f },
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

    glGenVertexArrays(1, &newBlock.VAO);
    glGenBuffers(1, &newBlock.VBO);
    glGenBuffers(1, &newBlock.EBO);

    glBindVertexArray(newBlock.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, newBlock.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(newBlock.vertices), &newBlock.vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newBlock.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(newBlock.indices), newBlock.indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    return newBlock;
}

Chunk newChunk() {
    Chunk newChunk;
    for(int z = 0; z < CHUNK_SIZE; z++) {
        for(int y = 0; y < CHUNK_SIZE; y++) {
            for(int x = 0; x < CHUNK_SIZE; x++) {
                newChunk.blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = newBlock((float)x*BLOCK_SIZE, (float)y*BLOCK_SIZE, (float)z*BLOCK_SIZE);
            }
        }
    }

    return newChunk;
}

void renderBlock(Block block, int mode) {
    glBindVertexArray(block.VAO);
    glDrawElements(mode, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void renderChunk(Chunk chunk, int mode) {
    for(int i = 0; i < CHUNK_VOL; i++) {
        renderBlock(chunk.blocks[i], mode);
    }
}


