/****************************** 
 * 
 * 
 * ***header
 * main Mesh data config...
 * 
 * 
 * ***********************/
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
#include <texture.hpp>
#include <array>
#include <cstddef>
#include <lighting.hpp>

class Main;
class Shader;
class Camera;

/**********
 * 
 * Uniform Buffer
 * 
 *******/
struct UniformBuffer {
    Mat4 view;
    Mat4 proj;
    Vec3 cameraPos; 
    float padding0;
    Lights lights;
};
/**
 * 
 */

/***************
 * 
 *  
 * Vertex
 * 
 * 
 *  **********/
struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
    float color[3];

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription b{};
        b.binding = 0;
        b.stride = sizeof(Vertex);
        b.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return b;
    }

    static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 4> a{};
        
        // inPosition
        a[0].binding = 0;
        a[0].location = 0;
        a[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        a[0].offset = offsetof(Vertex, position);
        
        // inNormal
        a[1].binding = 0;
        a[1].location = 1;
        a[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        a[1].offset = offsetof(Vertex, normal);

        // inUv
        a[2].binding = 0;
        a[2].location = 2;
        a[2].format = VK_FORMAT_R32G32_SFLOAT;
        a[2].offset = offsetof(Vertex, uv);

        // inColor
        a[3].binding = 0;
        a[3].location = 3;
        a[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        a[3].offset = offsetof(Vertex, color);

        return a;
    }
};

/***************
 * 
 *  
 * Mesh
 * 
 * 
 *  **********/
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
        Vec3 minBounds{ 0,0,0 };
        Vec3 maxBounds{ 0,0,0 };
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

    /**********
     * 
     * Instance
     * 
     *******/
    struct InstanceEntry {
        uint32_t id;
        Mat4 model;
        std::string type;
        uint32_t texId      = 0;
        uint32_t hasTexture = 0;
        uint32_t hasLit     = 1;
    };
    /**
     * 
     */

    /*******
     * 
     * Bounds
     * 
     */
    struct Bounds {
        Vec3 min, max;
        Vec3 center() const { return { (min.x+max.x), (min.y+max.y), (min.z+max.z) }; }
        Vec3 size() const { return { max.x-min.x, max.y-min.y, max.z-min.z }; }
    };

    Bounds getBounds(const std::string& type) const;
    /**
     * 
     */

    std::unordered_map<std::string, BufferData> bufferMeshes;
    std::unordered_map<uint32_t, Transform>     transforms;
    std::unordered_map<uint32_t, std::string>   idToType;
    std::unordered_map<uint32_t, uint32_t>      textureIds;

    VkDevice device;
    VkCommandPool commandPool;
    VkQueue graphicsQueue;
    VkPhysicalDevice physicalDevice;
    uint32_t maxFrames;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    std::vector<InstanceEntry> pendingInstances;
    VkBuffer instanceBuffer = VK_NULL_HANDLE;
    VkDeviceMemory instanceBufferMemory = VK_NULL_HANDLE;
    VkDeviceSize instanceBufferSize = 0;
    VkDeviceSize instanceBufferRequestedSize = 0;

    uint32_t drawCalls = 0;

    Main& main;
    Shader& shader;
    Camera& camera;

    TextureLoader* texture = nullptr;
    LightingManager* lighting = nullptr;

    const Transform& getTransform(uint32_t id) const;
    const BufferData& getBufferData(const std::string& type) const;

    void createBuffer(VkDeviceSize size, 
                        VkBufferUsageFlags usage, 
                        VkMemoryPropertyFlags properties, 
                        VkBuffer& buffer, 
                        VkDeviceMemory& bufferMemory);
    void uploadMesh(const std::string& type, const MeshData& data);

    void renderInstances(VkPipelineLayout layout, VkDescriptorSet* sets, uint32_t setCount);

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
                    -0.5f, -0.5f,  0.5f,   0,0,1,   0,0,   1,0,0,
                    0.5f, -0.5f,  0.5f,   0,0,1,   1,0,   1,0,0,
                    0.5f,  0.5f,  0.5f,   0,0,1,   1,1,   1,0,0,
                    -0.5f,  0.5f,  0.5f,   0,0,1,   0,1,   1,0,0,

                    0.5f, -0.5f, -0.5f,   0,0,-1,  0,0,   0,0,1,
                    -0.5f, -0.5f, -0.5f,   0,0,-1,  1,0,   0,0,1,
                    -0.5f,  0.5f, -0.5f,   0,0,-1,  1,1,   0,0,1,
                    0.5f,  0.5f, -0.5f,   0,0,-1,  0,1,   0,0,1,

                    -0.5f,  0.5f,  0.5f,   0,1,0,   0,0,   0,1,0,
                    0.5f,  0.5f,  0.5f,   0,1,0,   1,0,   0,1,0,
                    0.5f,  0.5f, -0.5f,   0,1,0,   1,1,   0,1,0,
                    -0.5f,  0.5f, -0.5f,   0,1,0,   0,1,   0,1,0,

                    -0.5f, -0.5f, -0.5f,   0,-1,0,  0,0,   1,1,0,
                    0.5f, -0.5f, -0.5f,   0,-1,0,  1,0,   1,1,0,
                    0.5f, -0.5f,  0.5f,   0,-1,0,  1,1,   1,1,0,
                    -0.5f, -0.5f,  0.5f,   0,-1,0,  0,1,   1,1,0,

                    0.5f, -0.5f,  0.5f,   1,0,0,   0,0,   0,1,1,
                    0.5f, -0.5f, -0.5f,   1,0,0,   1,0,   0,1,1,
                    0.5f,  0.5f, -0.5f,   1,0,0,   1,1,   0,1,1,
                    0.5f,  0.5f,  0.5f,   1,0,0,   0,1,   0,1,1,

                    -0.5f, -0.5f, -0.5f,  -1,0,0,   0,0,   1,0,1,
                    -0.5f, -0.5f,  0.5f,  -1,0,0,   1,0,   1,0,1,
                    -0.5f,  0.5f,  0.5f,  -1,0,0,   1,1,   1,0,1,
                    -0.5f,  0.5f, -0.5f,  -1,0,0,   0,1,   1,0,1,
                },
                // Indices
                {
                    0,1,2, 0,2,3,  4,5,6, 4,6,7,
                    8,9,10, 8,10,11,  12,13,14, 12,14,15,
                    16,17,18, 16,18,19,  20,21,22, 20,22,23
                },
                // Min Bounds
                Vec3(-0.5f, -0.5f, -0.5f),
                // Max Bounds
                Vec3(0.5f, 0.5f, 0.5f)
            });
            // Sphere
            m.emplace("sphere", []() -> MeshData {
                const int subdivisions = 4;
                const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

                auto norm = [](Vec3 v) {
                    float l = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
                    return Vec3(v.x/l, v.y/l, v.z/l);
                };

                std::vector<Vec3> positions = {
                    norm(Vec3(-1, t, 0)),   norm(Vec3(1, t, 0)),
                    norm(Vec3(-1, -t, 0)),  norm(Vec3(1, -t, 0)),
                    norm(Vec3(0, -1, t)),   norm(Vec3(0, 1, t)),
                    norm(Vec3(0, -1, -t)),  norm(Vec3(0, 1, -t)),
                    norm(Vec3(t, 0, -1)),   norm(Vec3(t, 0, 1)),
                    norm(Vec3(-t, 0, -1)),  norm(Vec3(-t, 0, 1))
                };
                std::vector<uint32_t> indices = {
                    0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
                    1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
                    3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
                    4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1
                };

                std::map<std::pair<uint32_t, uint32_t>, uint32_t> midpointCache;
                auto getMidpoint = [&](uint32_t i1, uint32_t i2) -> uint32_t {
                    auto key = i1 < i2 ? std::make_pair(i1, i2) : std::make_pair(i2, i1);
                    
                    auto it = midpointCache.find(key);
                    if(it != midpointCache.end()) return it->second;
                
                    Vec3 a = positions[i1];
                    Vec3 b = positions[i2];
                    Vec3 mid = norm(Vec3(a.x + b.x, a.y + b.y, a.z + b.z));

                    uint32_t idx = static_cast<uint32_t>(positions.size());
                    positions.push_back(mid);

                    midpointCache[key] = idx;
                    return idx;
                };

                for(int i = 0; i < subdivisions; ++i) {
                    std::vector<uint32_t> newIndices;

                    for(size_t j = 0; j < indices.size(); j += 3) {
                        uint32_t v1 = indices[j];
                        uint32_t v2 = indices[j+1];
                        uint32_t v3 = indices[j+2];

                        uint32_t a = getMidpoint(v1, v2);
                        uint32_t b = getMidpoint(v2, v3);
                        uint32_t c = getMidpoint(v3, v1);

                        newIndices.push_back(v1); newIndices.push_back(a); newIndices.push_back(c);
                        newIndices.push_back(v2); newIndices.push_back(b); newIndices.push_back(a);
                        newIndices.push_back(v3); newIndices.push_back(c); newIndices.push_back(b);
                        newIndices.push_back(a);  newIndices.push_back(b); newIndices.push_back(c);
                    }

                    indices = newIndices;
                    midpointCache.clear();
                }

                std::vector<float> vertices;
                std::vector<uint32_t> finalIndices;
                for(size_t i = 0; i < indices.size(); i += 3) {
                    Vec3 v0 = positions[indices[i]];
                    Vec3 v1 = positions[indices[i+1]];
                    Vec3 v2 = positions[indices[i+2]];

                    float u0 = 0.5f + std::atan2(v0.x, v0.z) / (2.0f * PI);
                    float y0 = 0.5f + std::asin(v0.y) / PI;
                    float u1 = 0.5f + std::atan2(v1.x, v1.z) / (2.0f * PI);
                    float y1 = 0.5f + std::asin(v1.y) / PI;
                    float u2 = 0.5f + std::atan2(v2.x, v2.z) / (2.0f * PI);
                    float y2 = 0.5f + std::asin(v2.y) / PI;

                    float uvs[3] = {u0, u1, u2};
                    for(int a = 0; a < 3; ++a) {
                        for(int b = a + 1; b < 3; ++b) {
                            if(std::abs(uvs[a] - uvs[b]) > 0.5f) {
                                if(uvs[a] < 0.5f) uvs[a] += 1.0f;
                                else if(uvs[b] < 0.5f) uvs[b] += 1.0f;
                            }
                        }
                    }
                    u0 = uvs[0];
                    u1 = uvs[1];
                    u2 = uvs[2];

                    uint32_t base = static_cast<uint32_t>(vertices.size() / Mesh::kVertexStride);
                    auto pushVertex = [&](const Vec3& p, float u, float v) {
                        vertices.push_back(p.x * 0.5f);
                        vertices.push_back(p.y * 0.5f);
                        vertices.push_back(p.z * 0.5f);
                        vertices.push_back(p.x);
                        vertices.push_back(p.y);
                        vertices.push_back(p.z);
                        vertices.push_back(u);
                        vertices.push_back(v);
                        vertices.push_back(0.3f); // color R
                        vertices.push_back(0.5f); // color G
                        vertices.push_back(0.8f); // color B
                    };
                    pushVertex(v0, u0, y0);
                    pushVertex(v1, u1, y1);
                    pushVertex(v2, u2, y2);

                    finalIndices.push_back(base);
                    finalIndices.push_back(base+1);
                    finalIndices.push_back(base+2);
                }

                return MeshData{
                    vertices, finalIndices,
                    Vec3(-0.5f, -0.5f, -0.5f),
                    Vec3(0.5f, 0.5f, 0.5f)
                };
            }());

            return m;
        }();
        return map;
    }
    /*********** */

public:
    Mesh(Main& main, Shader& shader, Camera& camera);
    ~Mesh();

    inline static constexpr size_t kVertexStride = sizeof(Vertex) / sizeof(float);
    const std::vector<void*> getUniformBuffersMapped() const { return uniformBuffersMapped; } 

    std::unordered_map<uint32_t, uint32_t> hasLitIds;
    void setHasLit(uint32_t id, bool lit);
    
    const Camera& getCamera() const;

    TextureLoader* getTextureLoader() const;
    void setTexture(uint32_t id, uint32_t texId);
    uint32_t getTexture(uint32_t id) const;

    LightingManager* getLightingManager() const;

    const std::vector<float>* getVertices(const std::string type) const;
    std::string getType(uint32_t id) const;
    Vec3 getPosition(uint32_t id) const;
    void setPosition(uint32_t id, float x, float y, float z);
    void setRotation(uint32_t id, float x, float y, float z);
    void setScale(uint32_t id, float x, float y, float z);
    void setColor(uint32_t id, float r, float g, float b);

    void setInit(VkDevice device,
            VkCommandPool commandPool,
            VkQueue graphicsQueue,
            VkPhysicalDevice physicalDevice,
            uint32_t maxFrames);
    void setInstanceBuffer(VkDeviceSize size);
    void load(uint32_t id, const std::string& type);
    void init();
    void create();
    void render(bool shadowPass = false);
    void renderShadowPass(VkCommandBuffer cmd);

    void cleanup();
    void clearInstances(const std::vector<uint32_t>& ids);

    /**
     * Begin Frame
     */
    void beginFrame() {
        drawCalls = 0;

        const size_t stride = sizeof(Mat4) + sizeof(uint32_t) * 3;
        VkDeviceSize needed = stride * VkDeviceSize(pendingInstances.size());
        if(needed > instanceBufferSize) {
            if(instanceBuffer != VK_NULL_HANDLE) {
                vkDeviceWaitIdle(device);
                vkDestroyBuffer(device, instanceBuffer, nullptr);
                vkFreeMemory(device, instanceBufferMemory, nullptr);
            }
            createBuffer(needed,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        instanceBuffer, instanceBufferMemory);
            instanceBufferSize = needed;
        }
    }

    /**
     * Get Draw Calls
     */
    uint32_t getDrawCalls() const {
        return drawCalls;
    }

    /********* 
     * Copy Buffer
     * *******/
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }

    /********* 
     * Find Memory Type
     * *******/
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    /********* 
     * Create Descriptor Set Layout
     * *******/
    void createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboBinding{};
        uboBinding.binding = 0;
        uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboBinding.descriptorCount = 1;
        uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

        VkDescriptorSetLayoutCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.bindingCount = 1;
        info.pBindings = &uboBinding;

        if(vkCreateDescriptorSetLayout(device, &info, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    /********* 
     * Create Uniform Buffer
     * *******/
    void createUniformBuffers() {
        VkDeviceSize size = sizeof(UniformBuffer);
        uniformBuffers.resize(maxFrames);
        uniformBuffersMemory.resize(maxFrames);
        uniformBuffersMapped.resize(maxFrames);

        for(size_t i = 0; i < maxFrames; i++) {
            VkBufferCreateInfo bi{};
            bi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bi.size = size;
            bi.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            vkCreateBuffer(device, &bi, nullptr, &uniformBuffers[i]);

            VkMemoryRequirements req;
            vkGetBufferMemoryRequirements(device, uniformBuffers[i], &req);

            VkMemoryAllocateInfo ai{};
            ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            ai.allocationSize = req.size;
            ai.memoryTypeIndex = findMemoryType(req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkAllocateMemory(device, &ai, nullptr, &uniformBuffersMemory[i]);
            vkBindBufferMemory(device, uniformBuffers[i], uniformBuffersMemory[i], 0);
            vkMapMemory(device, uniformBuffersMemory[i], 0, size, 0, &uniformBuffersMapped[i]);
        }
    }

    /********* 
     * Create Descriptor Pool
     * *******/
    void createDescriptorPool() {
        VkDescriptorPoolSize size{};
        size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        size.descriptorCount = static_cast<uint32_t>(maxFrames);

        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.poolSizeCount = 1;
        info.pPoolSizes = &size;
        info.maxSets = static_cast<uint32_t>(maxFrames);
        if(vkCreateDescriptorPool(device, &info, nullptr, &descriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    /********* 
     * Create Descriptor Sets
     * *******/
    void createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(maxFrames, descriptorSetLayout);
        
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = descriptorPool;
        info.descriptorSetCount = static_cast<uint32_t>(maxFrames);
        info.pSetLayouts = layouts.data();

        descriptorSets.resize(maxFrames);
        if(vkAllocateDescriptorSets(device, &info, descriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for(size_t i = 0; i < maxFrames; i++) {
            VkDescriptorBufferInfo buf{};
            buf.buffer = uniformBuffers[i];
            buf.offset = 0;
            buf.range = sizeof(UniformBuffer);

            VkWriteDescriptorSet w{};
            w.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            w.dstSet = descriptorSets[i];
            w.dstBinding = 0;
            w.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            w.descriptorCount = 1;
            w.pBufferInfo = &buf;

            vkUpdateDescriptorSets(device, 1, &w, 0, nullptr);
        }
    }

    /********* 
     * Get Vertex Input State
     * *******/
    VkPipelineVertexInputStateCreateInfo getVertexInputState() const {
        const std::size_t instAttr = 7;
        const std::size_t combinedArray = 11;

        static auto bindingDesc = Vertex::getBindingDescription();
        static auto attrDesc = Vertex::getAttributeDescriptions();

        static VkVertexInputBindingDescription instanceBinding{};
        instanceBinding.binding = 1;
        instanceBinding.stride = sizeof(Mat4) + sizeof(uint32_t) * 3;
        instanceBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        static std::array<VkVertexInputAttributeDescription, instAttr> instanceAttr{};
        for(int i = 0; i < 4; i++) {
            instanceAttr[i].binding = 1;
            instanceAttr[i].location = 4 + i;
            instanceAttr[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            instanceAttr[i].offset = sizeof(float) * 4 * i;
        }
        // location 8: texId
        instanceAttr[4].binding = 1;
        instanceAttr[4].location = 8;
        instanceAttr[4].format = VK_FORMAT_R32_UINT;
        instanceAttr[4].offset = sizeof(Mat4);
        // location 9: hasTexture
        instanceAttr[5].binding = 1;
        instanceAttr[5].location = 9;
        instanceAttr[5].format = VK_FORMAT_R32_UINT;
        instanceAttr[5].offset = sizeof(Mat4) + sizeof(uint32_t);
        // location 10: hasLit
        instanceAttr[6].binding = 1;
        instanceAttr[6].location = 10;
        instanceAttr[6].format = VK_FORMAT_R32_UINT;
        instanceAttr[6].offset = sizeof(Mat4) + sizeof(uint32_t) * 2;

        static std::array<VkVertexInputBindingDescription, 2> bindings = {
            bindingDesc,
            instanceBinding
        };
        static std::array<VkVertexInputAttributeDescription, combinedArray> allAttrs{};
        for(int i = 0; i < 4; i++) allAttrs[i] = attrDesc[i];
        for(int i = 0; i < instAttr; i++) allAttrs[4+i] = instanceAttr[i];

        VkPipelineVertexInputStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
        info.pVertexBindingDescriptions = bindings.data();
        info.vertexAttributeDescriptionCount = static_cast<uint32_t>(allAttrs.size());
        info.pVertexAttributeDescriptions = allAttrs.data();
        return info;
    }
    
    /********* 
     * Get Descriptor Set Layout
     * *******/
    VkDescriptorSetLayout getDescriptorSetLayout() const {
        return descriptorSetLayout;
    }

    /**********
     * Get Layouts
     *******/
    VkDescriptorSetLayout getLayouts() {
        VkDescriptorSetLayout map = {
            LightingManager::ShadowMap::shadowDescriptorSetLayout
        };
        return map;
    }
};