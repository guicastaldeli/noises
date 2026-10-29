#include <heightmap.hpp>

/************
 * 
 * Generate Heightmap
 * 
 *****/
Heightmap Heightmap::generateHeightmap(int sx, int sz, const NoiseParams& p) {
    Heightmap h(sx, sz);
    if(sx <= 0 || sz <= 0) return h;

    for(int z = 0; z < sz; ++z) {
        for(int x = 0; x < sx; ++x) {
            float v = Noise::sampleNoise2(p, float(x), float(z));
            float t = v * 0.5f + 0.5f;
            if(t < 0.0f) t = 0.0f;
            if(t > 1.0f) t = 1.0f;
            h.at(x, z) = p.minHeight + t * (p.maxHeight - p.minHeight);
        }
    }

    return h;
}