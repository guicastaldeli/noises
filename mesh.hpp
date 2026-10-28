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
#include <array>
#include <cstddef>

class Main;
class Camera;

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
     * Uniform Buffer
     * 
     *******/
    struct UniformBuffer {
        Mat4 view;
        Mat4 proj;
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
        Mat4 model;
        std::string type;
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

    uint32_t drawCalls = 0;

    Main& main;
    Camera& camera;

    const Transform& getTransform(uint32_t id) const;
    const BufferData& getBufferData(const std::string& type) const;

    void createBuffer(VkDeviceSize size, 
                        VkBufferUsageFlags usage, 
                        VkMemoryPropertyFlags properties, 
                        VkBuffer& buffer, 
                        VkDeviceMemory& bufferMemory);
    void uploadMesh(const std::string& type, const MeshData& data);

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
            //Vertices
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
            return m;
        }();
        return map;
    }
    /*********** */

public:
    Mesh(Main& main, Camera& camera);
    ~Mesh();

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
    void render();

    void cleanup();

    /**
     * Begin Frame
     */
    void beginFrame() {
        drawCalls = 0;
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
        static auto bindingDesc = Vertex::getBindingDescription();
        static auto attrDesc = Vertex::getAttributeDescriptions();

        static VkVertexInputBindingDescription instanceBinding{};
        instanceBinding.binding = 1;
        instanceBinding.stride = sizeof(Mat4);
        instanceBinding.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        static std::array<VkVertexInputAttributeDescription, 4> instanceAttr{};
        for(int i = 0; i < 4; i++) {
            instanceAttr[i].binding = 1;
            instanceAttr[i].location = 4 + i;
            instanceAttr[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
            instanceAttr[i].offset = sizeof(float) * 4 * i;
        }

        static std::array<VkVertexInputBindingDescription, 2> bindings = {
            bindingDesc,
            instanceBinding
        };
        static std::array<VkVertexInputAttributeDescription, 8> allAttrs{};
        for(int i = 0; i < 4; i++) allAttrs[i] = attrDesc[i];
        for(int i = 0; i < 4; i++) allAttrs[4+i] = instanceAttr[i];

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
};