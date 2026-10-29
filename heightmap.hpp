#pragma once

#include <noise.hpp>
#include <vector>

class Heightmap {
private:
    int m_sx = 0, m_sz = 0;
    std::vector<float> m_data;

public:
    Heightmap() = default;
    Heightmap(int sx, int sz) :
        m_data(std::size_t(sx) * std::size_t(sz), 0.0f),
        m_sx(sx),
        m_sz(sz) {};

    int sx() const { return m_sx; }
    int sz() const { return m_sz; }
    float at(int x, int z) const { return m_data[std::size_t(x) + std::size_t(z) * std::size_t(m_sz)]; }
    float& at(int x, int z) { return m_data[std::size_t(x) + std::size_t(z) * std::size_t(m_sx)]; }
    void clear() { m_sx = m_sz = 0; m_data.clear(); }
    
    // Generates a heightmap of size (sx, sz)
    // range [params.minHeight, params.maxHeight]
    static Heightmap generateHeightmap(int sx, int sz, const NoiseParams& params);
};
