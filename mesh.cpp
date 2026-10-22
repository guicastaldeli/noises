#include <mesh.hpp>
#include <cake.hpp>
#include <cstring>

Mesh::Mesh(Main& main) : main(main) {};
Mesh::~Mesh() {};

/**
 * Set Position
 */
void Mesh::setPosition(uint32_t id, float x, float y, float z) {
    transforms.at(id).position = Vec3(x, y, z);
}

/**
 * Set Rotation
 */
void Mesh::setRotation(uint32_t id, float x, float y, float z) {
    transforms.at(id).rotation = Vec3(x, y, z);
}

/**
 * Set Scale
 */
void Mesh::setScale(uint32_t id, float x, float y, float z) {
    transforms.at(id).scale = Vec3(x, y, z);
}

/**
 * Set Color
 */
void Mesh::setColor(uint32_t id, float r, float g, float b) {
    transforms.at(id).color = Vec3(r, g, b);
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
    vkMapMemory(main.device, vertexStagingMem, 0, vertexSize, 0, &vData);
    memcpy(vData, data.vertices.data(), (size_t)vertexSize);
    vkUnmapMemory(main.device, vertexStagingMem);

    void* iData;
    vkMapMemory(main.device, indexStagingMem, 0, indexSize, 0, &iData);
    memcpy(iData, data.indices.data(), (size_t)indexSize);
    vkUnmapMemory(main.device, indexStagingMem);

    createBuffer(vertexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferData.vertexBuffer, bufferData.vertexBufferMemory);
    createBuffer(indexSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferData.indexBuffer, bufferData.indexBufferMemory);
    
    copyBuffer(vertexStaging, bufferData.vertexBuffer, vertexSize);
    copyBuffer(indexStaging, bufferData.indexBuffer, indexSize);

    vkDestroyBuffer(main.device, vertexStaging, nullptr);
    vkFreeMemory(main.device, vertexStagingMem, nullptr);
    vkDestroyBuffer(main.device, indexStaging, nullptr);
    vkFreeMemory(main.device, indexStagingMem, nullptr);

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
    if(vkCreateBuffer(main.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(main.device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    if(vkAllocateMemory(main.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(main.device, buffer, bufferMemory, 0);
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
    ubo.model = t.GetModelMatrix();
    ubo.color = t.color;

    std::memcpy(uniformBuffersMapped[main.currentFrame], &ubo, sizeof(ubo));

    VkDescriptorSet ds = descriptorSets[main.currentFrame];
    VkCommandBuffer cmd = main.getCurrentCommandBuffer();
    const BufferData& bufferData = bufferMeshes.at(type);

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
        vkDestroyBuffer(main.device, pair.second.vertexBuffer, nullptr);
        vkFreeMemory(main.device, pair.second.vertexBufferMemory, nullptr);
        vkDestroyBuffer(main.device, pair.second.indexBuffer, nullptr);
        vkFreeMemory(main.device, pair.second.indexBufferMemory, nullptr);
    }

    bufferMeshes.clear();
    transforms.clear();
    idToType.clear();
}
