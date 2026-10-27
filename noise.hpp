/************************************** 
 * 
 * 
 * General noise and related types
 * and noise implementation...
 * 
 * 
 * ******************************/
#pragma once

#include <cstdint>
#include <vector>

/******
 * 
 * Noise Type
 * 
 */
enum class NoiseType {
    Value, 
    ValueFractual,
    Perlin,
    PerlinFractual,
    Cellular,
    WhiteNoise,
    Cubic,
    CubicFractual
};

/****** 
 * 
 * Interp Type
 * 
 * ***/
enum class InterpType {
    Linear,
    Hermite,
    Quintic
};

/*********
 * 
 * Fractal Type
 * 
 */
enum class FractalType {
    FBM,
    Billow,
    RigitMulti
};

/***************
 * 
 * Cellular Distance
 * 
 *****/
enum class CellularDistance {
    Euclidean,
    Manhattan,
    Natural
};

/**** 
 * 
 * Cellular Return
 * 
 * ****/
enum class CellularReturn {
    CellValue,
    NoiseLookup,
    Distance,
    Distance2,
    Distance2Add,
    Distance2Sub,
    Distance2Mul,
    Distance2Div
};

/*********************
 * 
 * Noise Params
 * 
 */
struct NoiseParams {
    /**
     * Data types
     */
    NoiseType type =                NoiseType::Perlin;
    InterpType interp =             InterpType::Quintic;
    FractalType fractal =           FractalType::FBM;
    CellularDistance cellDist =     CellularDistance::Euclidean;
    CellularReturn cellRet =        CellularReturn::Distance;
    /**
     * 
     */

    float frequency = 0.01f;
    int seed = 1337;
    int octaves = 4;
    float lacunary = 2.0f;
    float gain = 0.5f;

    float minHeight = 0.0f;
    float maxHeight = 64.0f;
};

/********* 
 * 
 * Noise methods.
 * 
 * *****/
class Noise {
    // One sample of noise at (x, z)
    static float sampleNoise(const NoiseParams& p, float x, float y, float z);
    
    // One sample at 2D coords
    inline static float sampleNoise2(const NoiseParams& p, float x, float z) {
        return sampleNoise(p, x, 0.0f, z);
    }
};