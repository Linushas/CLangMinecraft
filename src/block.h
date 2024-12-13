#ifndef BLOCK_H
#define BLOCK_H

#define POS(x,y,z) (float[]){x,y,z}

#define CHUNK_SIZE 8
#define CHUNK_VOL CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE
#define BLOCK_SIZE 1.0f

typedef struct vertex {
    float x, y, z;
    float r, g, b;
    float tx, ty, yz;
    float nx, ny, nz;
} Vertex;

typedef struct block {
    float x, y, z;
    float r, g, b; 
    Vertex vertices[24];
    unsigned int indices[36];
    unsigned int VAO, VBO, EBO;
} Block;

typedef struct chunk {
    float x, y, z;
    Block blocks[CHUNK_VOL];
} Chunk;

Block newBlock(float x, float y, float z);
Chunk newChunk();
void renderBlock(Block block, int mode);
void renderChunk(Chunk chunk, int mode);

#endif