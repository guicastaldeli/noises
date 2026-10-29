/****************************** 
 * 
 * 
 * main Mesh data config...
 * 
 * 
 * ***********************/
#include <mesh.hpp>
#include <cake.hpp>
#include <camera.hpp>
#include <cstring>

Mesh::Mesh(Main& main, Camera& camera) : 
    main(main),
    camera(camera) {};
Mesh::~Mesh() {};

/**
 * Get Vertices
 */
const std::vector<float>* Mesh::getVertices(const std::string type) const {
    const auto& dataMap = Data();
    auto it = dataMap.find(type);
    if(it == dataMap.end()) return nullptr;
    return &it->second.vertices;
}

/**
 * Get Type
 */
std::string Mesh::getType(uint32_t id) const {
    auto it = idToType.find(id);
    return (it == idToType.end()) ? std::string{} : it->second;
}

/**
 * Get Position
 */
Vec3 Mesh::getPosition(uint32_t id) const {
    auto it = transforms.find(id);
    if(it == transforms.end()) return Vec3(0.0f, 0.0f, 0.0f);
    return it->second.position;
}

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
    t.rotation.x += x;
    t.rotation.y += y;
    t.rotation.z += z;
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

    Vec3 mn(std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max());
    Vec3 mx(-std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max());

    const size_t stride = 11;
    for(size_t i = 0; i+2 < data.vertices.size(); i += stride) {
        float x = data.vertices[i+0];
        float y = data.vertices[i+1];
        float z = data.vertices[i+2];
        if(x < mn.x) mn.x = x; if(x > mx.x) mx.x = x;
        if(y < mn.y) mn.y = y; if(y > mx.y) mx.y = y;
        if(z < mn.z) mn.z = z; if(z > mx.z) mx.z = z;
    }

    bufferData.minBounds = mn;
    bufferData.maxBounds = mx;

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
 * Get Bounds
 */
Mesh::Bounds Mesh::getBounds(const std::string& type) const {
    auto it = bufferMeshes.find(type);
    if(it != bufferMeshes.end()) {
        return { it->second.minBounds, it->second.maxBounds };
    }

    const auto& dataMap = Data();
    auto dit = dataMap.find(type);
    if(dit == dataMap.end()) throw std::runtime_error("Mesh::getBounds: unknown type " + type);

    return { dit->second.minBounds, dit->second.maxBounds }; 
}

/**
 * Set Init
 */
void Mesh::setInit(VkDevice device,
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
 * Set Instance Buffer
 */
void Mesh::setInstanceBuffer(VkDeviceSize size) {
    if(instanceBuffer != VK_NULL_HANDLE && instanceBufferSize >= size) return;
    if(instanceBuffer != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(device);
        vkDestroyBuffer(device, instanceBuffer, nullptr);
        vkFreeMemory(device, instanceBufferMemory, nullptr);
    }

    createBuffer(size,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                instanceBuffer, instanceBufferMemory);
    
    instanceBufferSize = size;
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
    Mat4 model = t.GetModelMatrix();

    InstanceEntry entry{};
    entry.id = id;
    entry.model = model;
    entry.type = type;
    pendingInstances.push_back(entry);
}

/**
 * 
 * Render
 * 
 */
void Mesh::render() {
    if(pendingInstances.empty()) return;

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

    std::unordered_map<std::string, std::vector<Mat4>> byType;
    for(auto& entry : pendingInstances) {
        auto tit = transforms.find(entry.id);
        if(tit == transforms.end()) continue;

        Mat4 model = tit->second.GetModelMatrix();
        Mat4 transported{};
        for(int r = 0; r < 4; ++r)
        for(int c = 0; c < 4; ++c) {
            transported.m[r][c] = model.m[c][r];
        }

        byType[entry.type].push_back(transported);
    }

    VkCommandBuffer cmd = main.getCurrentCommandBuffer();
    VkDescriptorSet ds = descriptorSets[main.currentFrame];
    vkCmdBindDescriptorSets(cmd,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            main.pipelineLayout,
                            0, 1, &ds,
                            0, nullptr);
    
    for(auto& [type, models] : byType) {
        VkDeviceSize size = sizeof(Mat4) * models.size();
        setInstanceBuffer(size);

        void* data;
        vkMapMemory(device, instanceBufferMemory, 0, size, 0, &data);
        memcpy(data, models.data(), size);
        vkUnmapMemory(device, instanceBufferMemory);

        const BufferData& bufferData = bufferMeshes.at(type);

        VkBuffer vbs[] = { bufferData.vertexBuffer, instanceBuffer };
        VkDeviceSize offsets[] = { 0, 0 };
        vkCmdBindVertexBuffers(cmd, 0, 2, vbs, offsets);
        vkCmdBindIndexBuffer(cmd, bufferData.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(cmd, bufferData.indexCount,
                        static_cast<uint32_t>(models.size()),
                        0, 0, 0);

        drawCalls++;
    }

    //pendingInstances.clear();
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

/**
 * Clear Instances
 */
void Mesh::clearInstances(const std::vector<uint32_t>& ids) {
    std::unordered_set<uint32_t> removeSet(ids.begin(), ids.end());
    for(uint32_t id : ids) {
        transforms.erase(id);
        idToType.erase(id);
    }

    pendingInstances.erase(
        std::remove_if(pendingInstances.begin(), pendingInstances.end(),
                        [&](const InstanceEntry& e) {
                            return removeSet.count(e.id) > 0;
                        }),
                        pendingInstances.end());
}
