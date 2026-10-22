#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdexcept>
#include <map>
#include <cmath>
#include <utils/vec3.hpp>
#include <utils/mat4.hpp>
#include <utils/transform.hpp>

class Main;
class Mesh {
private:
    /*********
     * 
     * Buffer Data
     * 
     */
    struct BufferData {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        uint32_t indexCount;
    };
    /**
     *
     */

    /**********
     * 
     * Mesh Data
     * 
     *******/
    struct MeshData {
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        Vec3 minBounds;
        Vec3 maxBounds;
    };
    /**
     * 
     */

    std::unordered_map<std::string, BufferData> bufferMeshes;
    std::unordered_map<uint32_t, Transform> transforms;
    std::unordered_map<uint32_t, std::string> idToType;

    Main& main;

    const Transform& getTransform(uint32_t id) const;
    const BufferData& getBufferData(const std::string& type) const;

    void createBuffer(VkDeviceSize size, 
                        VkBufferUsageFlags usage, 
                        VkMemoryPropertyFlags properties, 
                        VkBuffer& buffer, 
                        VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void uploadMesh(const std::string& type, const MeshData& data);
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    /********************** 
     * 
     * Data
     * 
     * ***********/
    static const std::unordered_map<std::string, MeshData>& Data() {
        static const std::unordered_map<std::string, MeshData> map = []() {
            std::unordered_map<std::string, MeshData> m;
            // Triangle
            m.emplace("triangle", MeshData{
                // Vertices
                {
                    -0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,     0.0f, 0.0f,
                     0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,     1.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f,     1.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f,     0.0f, 1.0f,
                    
                     0.0f,  0.5f,  0.0f,    -0.894f, 0.447f, 0.0f,   0.5f, 0.5f,
                     0.0f,  0.5f,  0.0f,     0.0f, 0.447f, -0.894f,  0.5f, 0.5f, 
                     0.0f,  0.5f,  0.0f,     0.894f, 0.447f, 0.0f,   0.5f, 0.5f,
                     0.0f,  0.5f,  0.0f,     0.0f, 0.447f, 0.894f,   0.5f, 0.5f,
                },
                // Indices
                {
                    4, 0, 1,
                    5, 1, 2,
                    6, 2, 3,
                    7, 3, 0,
                    
                    0, 1, 2,
                    0, 2, 3
                },
                // Min Bounds
                Vec3(-0.5f, -0.5f, -0.5f),
                // Max Bounds
                Vec3(0.5f, 0.5f, 0.5f)
            });
            // Cube
            m.emplace("cube", MeshData{
                // Vertices
                {
                    -0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,     0.0f, 0.0f, 1.0f,     1.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,     1.0f, 1.0f,
                    -0.5f,  0.5f,  0.5f,     0.0f, 0.0f, 1.0f,     0.0f, 1.0f,
                    
                     0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,    0.0f, 0.0f,
                    -0.5f, -0.5f, -0.5f,     0.0f, 0.0f, -1.0f,    1.0f, 0.0f,
                    -0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,    1.0f, 1.0f,
                     0.5f,  0.5f, -0.5f,     0.0f, 0.0f, -1.0f,    0.0f, 1.0f,
                    
                    -0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,     0.0f, 0.0f,
                     0.5f,  0.5f,  0.5f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,     1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,     0.0f, 1.0f, 0.0f,     0.0f, 1.0f,
                    
                    -0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,    0.0f, 0.0f,
                     0.5f, -0.5f, -0.5f,     0.0f, -1.0f, 0.0f,    1.0f, 0.0f,
                     0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f,    1.0f, 1.0f,
                    -0.5f, -0.5f,  0.5f,     0.0f, -1.0f, 0.0f,    0.0f, 1.0f,
                    
                     0.5f, -0.5f,  0.5f,     1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
                     0.5f, -0.5f, -0.5f,     1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
                     0.5f,  0.5f, -0.5f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
                     0.5f,  0.5f,  0.5f,     1.0f, 0.0f, 0.0f,     0.0f, 1.0f,
                    
                    -0.5f, -0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,     0.0f, 0.0f,
                    -0.5f, -0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,     1.0f, 0.0f,
                    -0.5f,  0.5f,  0.5f,    -1.0f, 0.0f, 0.0f,     1.0f, 1.0f,
                    -0.5f,  0.5f, -0.5f,    -1.0f, 0.0f, 0.0f,     0.0f, 1.0f
                },
                // Indices
                {
                    0, 1, 2, 0, 2, 3,
                    4, 5, 6, 4, 6, 7,
                    8, 9, 10, 8, 10, 11,
                    12, 13, 14, 12, 14, 15,
                    16, 17, 18, 16, 18, 19,
                    20, 21, 22, 20, 22, 23
                },
                // Min Bounds
                Vec3(-0.5f, -0.5f, -0.5f),
                // Max Bounds
                Vec3(0.5f, 0.5f, 0.5f)
            });
            return m;
        }();
        return map;
    }
    /*********** */

public:
    Mesh(Main& main);
    ~Mesh();

    void setPosition(uint32_t id, float x, float y, float z);
    void setRotation(uint32_t id, float x, float y, float z);
    void setScale(uint32_t id, float x, float y, float z);
    void setColor(uint32_t id, float r, float g, float b);

    void load(uint32_t id, const std::string& type);

    void cleanup();
};