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
    SAND,
    TNT,
    SELECTED
};

typedef struct vertex {
    float x, y, z;
    float nx, ny, nz;
    float tx, ty;
} Vertex;

typedef struct face {
    Vertex vertices[4];
    uint8_t indices[6];
} Face;

typedef struct chunkMesh {
    float x, y, z;
    int indiceCount;
    unsigned int VAO, VBO, EBO;
} ChunkMesh;

typedef struct cubeMesh {
    Vertex vertices[24];
    uint8_t indices[36];
    int indiceCount;
    unsigned int VAO, VBO, EBO;
} CubeMesh;

typedef struct world World;
typedef struct RayHit RayHit;

Face newFace(float x, float y, float z, int faceID, int blockType);
ChunkMesh newChunk(World *world, float xPos, float yPos, float zPos, int chunkX, int chunkZ);
void selectedCube(RayHit hit);
void renderChunk(ChunkMesh chunk, int mode);

#endif