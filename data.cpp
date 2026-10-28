#include <data.hpp>
#include <chunk.hpp>
#include <terrain.hpp>

Data::Data(Terrain& terrain, UI& ui) :
    terrain(terrain),
    ui(ui) {};
Data::~Data() {};

/**
 * 
 * Init
 * 
 */
void Data::init() {
    chunk.type = ChunkType::Flat;
    chunk.sizeX = CHUNK_X_SIZE;
    chunk.sizeY = CHUNK_Y_SIZE;
    chunk.sizeZ = CHUNK_Z_SIZE;

    noise.type = NoiseType::PerlinFractual;
    noise.frequency = 0.03f;
    noise.seed = 1337;
    noise.octaves = 4;
    noise.lacunary = 2.0f;
    noise.gain = 0.5f;
    noise.minHeight = 0.0f;
    noise.maxHeight = float(chunk.sizeY);

    heightmap = Heightmap(chunk.sizeX, chunk.sizeZ);
    chunkData = Chunk::generateChunk(chunk, heightmap);
    hasTerrain = true;
    if(terrain != NULL) terrain.upload(chunkData);
}