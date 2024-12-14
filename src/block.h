#ifndef BLOCK_H
#define BLOCK_H

#define CHUNK_SIZE 16
#define CHUNK_VOL CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE
#define BLOCK_SIZE 1.0f

enum faceID{FRONT, BACK, LEFT, RIGHT, TOP, BOTTOM};

enum blockType{
    AIR,
    GRASS,
    DIRT,
    STONE,
    SAND
};

typedef struct vertex {
    float x, y, z;
    float nx, ny, nz;
    float tx, ty;
} Vertex;

typedef struct face {
    Vertex vertices[4];
    unsigned int indices[6];
} Face;

typedef struct block {
    int type;
} Block;

typedef struct chunk {
    float x, y, z;
    Block blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
    int indiceCount;
    unsigned int VAO, VBO, EBO;
} Chunk;

Face newFace(float x, float y, float z, int faceID);
Chunk newChunk(float x, float y, float z);
void renderChunk(Chunk chunk, int mode);

#endif