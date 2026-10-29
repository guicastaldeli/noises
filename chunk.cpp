#include <chunk.hpp>
#include <cmath>
#include <random>

/**************
 * 
 * Generate Chunk
 * 
 */
ChunkData Chunk::generateChunk(const ChunkParams& p, const Heightmap& heightmap) {
    ChunkData c;
    c.sx = p.sizeX;
    c.sy = p.sizeY;
    c.sz = p.sizeZ;
    c.meshes.assign(std::size_t(c.sx) * std::size_t(c.sy) * std::size_t(c.sz), 0);

    std::mt19937 rng(0xC0FFEEu);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for(int z = 0; z < c.sz; ++z) {
        for(int x = 0; x < c.sx; ++x) {
            float hm = heightmap.at(x, z);
            int hInt = int(std::lround(hm));
            if(hInt < 0) hInt = 0;
            if(hInt > c.sy) hInt = c.sy;

            switch(p.type) {
                case ChunkType::Full:
                    for(int y = 0; y < c.sy; ++y) {
                        c.at(x, y, z) = p.matSolid;
                    } 
                    break;
                case ChunkType::Flat:
                    for(int y = 0; y < hInt; ++y) {
                        c.at(x, y, z) = p.matSolid;
                    }
                    break;
                case ChunkType::Heightmap:
                    for(int y = 0; y < hInt; ++y) {
                        c.at(x, y, z) = (y == hInt - 1) ? p.matSurface : p.matSolid;
                    }
                    break;
                case ChunkType::Wave: {
                    float hz = std::sin(float(x) * p.waveFrequency) * p.waveAmplitude +
                                std::cos(float(z) * p.waveFrequency) * p.waveAmplitude;
                    float th = int(std::lround(float(hInt) + hz));
                    if(th < 0) th = 0;
                    if(th > c.sy) th = c.sy;
                    for(int y = 0; y < th; ++y) {
                        c.at(x, y, z) = p.matSolid;
                    }
                    break;
                }
                case ChunkType::Cheese:
                    for(int y = 0; y < c.sy; ++y) {
                        c.at(x, y, z) = (dist(rng) < p.cheeseDensity) ? p.matSolid : 0;
                    }
                    break;
                case ChunkType::Cave: {
                    for(int y = 0; y < hInt; y++) {
                        float n = heightmap.at(x, z) * 0.05f + float(y) * 0.15f;
                        float f = std::fmod(std::fabs(n), 1.0f);
                        c.at(x, y, z) = (f > p.caveThreshold) ? p.matSolid : 0;
                    }
                    break;
                }
            }
        }
    }
    return c;
}