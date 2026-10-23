#include <mesh.hpp>
#include <cake.hpp>
#include <camera.hpp>
#include <cstring>

Mesh::Mesh(Main& main, Camera& camera) : 
    main(main),
    camera(camera) {};
Mesh::~Mesh() {};

/**
 * Set Position
 */
void Mesh::setPosition(uint32_t id, float x, float y, float z) {
    auto& t = transforms[id];
    t.position = Vec3(x, y, z);
}

/**
 * Set Rotation
 */
void Mesh::setRotation(uint32_t id, float x, float y, float z) {
    auto& t = transforms[id];
    t.rotation = Vec3(x, y, z);
}

/**
 * Set Scale
 */
void Mesh::setScale(uint32_t id, float x, float y, float z) {
    auto& t = transforms[id];
    t.scale = Vec3(x, y, z);
}

/**
 * Set Color
 */
void Mesh::setColor(uint32_t id, float r, float g, float b) {
    transforms[id].color = Vec3(r, g, b);
}

/**
 * Get Transform
 */
const Transform& Mesh::getTransform(uint32_t id) const {
    return transforms.at(id);
}

/**
 * Get Buffer Data
 */
const Mesh::BufferData& Mesh::getBufferData(const std::string& type) const {
    return bufferMeshes.at(type);
}

/**
 * Get Data Size
 */
size_t Mesh::getInstanceByteSize() const {
   return sizeof(Instance);
}

/**
 * Upload Mesh
 */
void Mesh::uploadMesh(const std::string& type, const MeshData& data) {
    VkDeviceSize vertexSize = sizeof(float) * data.vertices.size();
    VkDeviceSize indexSize = sizeof(uint32_t) * data.indices.size();

    BufferData bufferData;
    bufferData.indexCount = static_cast<uint32_t>(data.indices.size());

    VkBuffer vertexStaging, indexStaging;
    VkDeviceMemory vertexStagingMem, indexStagingMem;

    createBuffer(vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexStaging, vertexStagingMem);
    createBuffer(indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indexStaging, indexStagingMem);

    void* vData;
    vkMapMemory(device, vertexStagingMem, 0, vertexSize, 0, &vData);
    memcpy(vData, data.vertices.data(), (size_t)vertexSize);
    vkUnmapMemory(device, vertexStagingMem);

    void* iData;
    vkMapMemory(device, indexStagingMem, 0, indexSize, 0, &iData);
    memcpy(iData, data.indices.data(), (size_t)indexSize);
    vkUnmapMemory(device, indexStagingMem);

    createBuffer(vertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferData.vertexBuffer, bufferData.vertexBufferMemory);
    createBuffer(indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferData.indexBuffer, bufferData.indexBufferMemory);
    
    copyBuffer(vertexStaging, bufferData.vertexBuffer, vertexSize);
    copyBuffer(indexStaging, bufferData.indexBuffer, indexSize);

    vkDestroyBuffer(device, vertexStaging, nullptr);
    vkFreeMemory(device, vertexStagingMem, nullptr);
    vkDestroyBuffer(device, indexStaging, nullptr);
    vkFreeMemory(device, indexStagingMem, nullptr);

    bufferMeshes[type] = bufferData;
}

/**
 * Create Buffer
 */
void Mesh::createBuffer(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory
) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    if(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

/**
 * Set
 */
void Mesh::set(VkDevice device,
            VkCommandPool commandPool,
            VkQueue graphicsQueue,
            VkPhysicalDevice physicalDevice,
            uint32_t maxFrames)
{
    this->device = device;
    this->commandPool = commandPool;
    this->graphicsQueue = graphicsQueue;
    this->physicalDevice = physicalDevice;
    this->maxFrames = maxFrames;
}

/**
 * 
 * Load
 * 
 */
void Mesh::load(uint32_t id, const std::string& type) {
    if(bufferMeshes.find(type) == bufferMeshes.end()) {
        const auto& dataMap = Data();
        auto it = dataMap.find(type);
        if(it == dataMap.end()) throw std::runtime_error("Unknown mesh type: " + type);
        uploadMesh(type, it->second);
    }

    if(transforms.find(id) == transforms.end()) {
        transforms[id] = Transform{};
        idToType[id] = type;
    }

    Transform& t = transforms.at(id);
    UniformBuffer ubo{};
    Mat4 view = camera.getViewMatrix();
    Mat4 proj = camera.getProjectionMatrix();
    for(int r = 0; r < 4; ++r) {
        for(int c = 0; c < 4; ++c) {
            ubo.view.m[r][c] = view.m[c][r];
            ubo.proj.m[r][c] = proj.m[c][r];
        }
    }

    std::memcpy(uniformBuffersMapped[main.currentFrame], &ubo, sizeof(ubo));

    Mat4 model = t.GetModelMatrix();
    Instance i{};
    for(int r = 0; r < 4; ++r) {
        for(int c = 0; c < 4; ++c) {
            i.model.m[r][c] = model.m[c][r];
        }
    }

    VkDescriptorSet ds = descriptorSets[main.currentFrame];
    VkCommandBuffer cmd = main.getCurrentCommandBuffer();
    const BufferData& bufferData = bufferMeshes.at(type);

    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            main.pipelineLayout,
                            0, 1, &ds,
                            0, nullptr);
    vkCmdPushConstants(cmd, main.pipelineLayout,
                        VK_SHADER_STAGE_VERTEX_BIT,
                        0, sizeof(Instance), &i);

    VkBuffer vbs[] = { bufferData.vertexBuffer };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, vbs, offsets);
    vkCmdBindIndexBuffer(cmd, bufferData.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmd, bufferData.indexCount, 1, 0, 0, 0);
}

/**
 * 
 * Init
 * 
 */
void Mesh::init() {
    createDescriptorSetLayout();
    createUniformBuffers();
    createDescriptorPool();
    createDescriptorSets();
}

/**
 * Cleanup
 */
void Mesh::cleanup() {
    for(auto& pair : bufferMeshes) {
        vkDestroyBuffer(device, pair.second.vertexBuffer, nullptr);
        vkFreeMemory(device, pair.second.vertexBufferMemory, nullptr);
        vkDestroyBuffer(device, pair.second.indexBuffer, nullptr);
        vkFreeMemory(device, pair.second.indexBufferMemory, nullptr);
    }

    for(size_t i = 0; i < uniformBuffers.size(); i++) {
        vkDestroyBuffer(device, uniformBuffers[i], nullptr);
        vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
    }
    uniformBuffers.clear();
    uniformBuffersMemory.clear();
    uniformBuffersMapped.clear();

    if(descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }
    if(descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        descriptorSetLayout = VK_NULL_HANDLE;
    }
    transforms.clear();
    idToType.clear();
}
