#include <texture.hpp>
#include <cake.hpp>
#include <stdexcept>
#include <cstring>
#include <vector>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <.lib/stb-master/stb_image.h>

TextureLoader::TextureLoader(Main& main) : main(main) {};
TextureLoader::~TextureLoader() {};

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, 
                        uint32_t typeFilter, 
                        VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);
    for(uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if((typeFilter & (1u << i)) &&
            (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
    }

    throw std::runtime_error("TextureLoader: no suitable memory type");
}

/**
 * 
 * Load
 * 
 */
uint32_t TextureLoader::load(uint32_t id, const std::string& path) {
    int w = 0, h = 0, ch = 0;
    stbi_uc* pixels = stbi_load(path.c_str(), &w, &h, &ch, STBI_rgb_alpha);
    if(!pixels) {
        throw std::runtime_error("TextureLoader: failed to load '" + path + "': " +
        (stbi_failure_reason() ? stbi_failure_reason() : "unknown"));
    }
    if(w <= 0 || h <= 0) {
        stbi_image_free(pixels);
        throw std::runtime_error("TextureLoader: '" + path + "' has invalid dimensions");
    }

    if(nextSlot >= maxTextures) {
        stbi_image_free(pixels);
        throw std::runtime_error("TextureLoader: descriptor array exhausted");
    }

    const uint32_t slot = nextSlot++;
    const VkDeviceSize imageSize = VkDeviceSize(w) * VkDeviceSize(h) * 4u;

    VkBuffer stagingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

    {
        VkBufferCreateInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bi.size = imageSize;
        bi.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bi.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if(vkCreateBuffer(device, &bi, nullptr, &stagingBuffer) != VK_SUCCESS) {
            stbi_image_free(pixels);
            throw std::runtime_error("TextureLoader: failed to create staging buffer");
        }

        VkMemoryRequirements req{};
        vkGetBufferMemoryRequirements(device, stagingBuffer, &req);

        VkMemoryAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        ai.allocationSize = req.size;
        ai.memoryTypeIndex = findMemoryType(physicalDevice, req.memoryTypeBits, 
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if(vkAllocateMemory(device, &ai, nullptr, &stagingMemory) != VK_SUCCESS) {
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            stbi_image_free(pixels);
            throw std::runtime_error("TextureLoader: failed to allocate staging memory");
        }

        vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);

        void* data = nullptr;
        vkMapMemory(device, stagingMemory, 0, imageSize, 0, &data);
        std::memcpy(data, pixels, (size_t)imageSize);
        vkUnmapMemory(device, stagingMemory);
    }

    stbi_image_free(pixels);

    VkImage image = VK_NULL_HANDLE;
    {
        VkImageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        ci.imageType = VK_IMAGE_TYPE_2D;
        ci.extent.width = (uint32_t)w;
        ci.extent.height = (uint32_t)h;
        ci.extent.depth = 1;
        ci.mipLevels = 1;
        ci.arrayLayers = 1;
        ci.format = VK_FORMAT_R8G8B8A8_SRGB;
        ci.tiling = VK_IMAGE_TILING_OPTIMAL;
        ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        ci.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                    VK_IMAGE_USAGE_SAMPLED_BIT;
        ci.samples = VK_SAMPLE_COUNT_1_BIT;
        ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if(vkCreateImage(device, &ci, nullptr, &image) != VK_SUCCESS) {
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingMemory, nullptr);
            throw std::runtime_error("TextureLoader: failed to create image");
        }
    }

    VkDeviceMemory imageMemory = VK_NULL_HANDLE;
    {
        VkMemoryRequirements req{};
        vkGetImageMemoryRequirements(device, image, &req);

        VkMemoryAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        ai.allocationSize = req.size;
        ai.memoryTypeIndex = findMemoryType(physicalDevice, req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if(vkAllocateMemory(device, &ai, nullptr, &imageMemory) != VK_SUCCESS) {
            vkDestroyImage(device, image, nullptr);
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingMemory, nullptr);
            throw std::runtime_error("TextureLoader: failed to allocate image memory");
        }
        vkBindImageMemory(device, image, imageMemory, 0);
    }

    {
        VkCommandBufferAllocateInfo ai{};
        ai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        ai.commandPool = commandPool;
        ai.commandBufferCount = 1;

        VkCommandBuffer cmd = VK_NULL_HANDLE;
        vkAllocateCommandBuffers(device, &ai, &cmd);

        VkCommandBufferBeginInfo bi{};
        bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(cmd, &bi);

        {
            VkImageMemoryBarrier b{};
            b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            b.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            b.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.image = image;
            b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            b.subresourceRange.baseMipLevel = 0;
            b.subresourceRange.levelCount = 1;
            b.subresourceRange.baseArrayLayer = 0;
            b.subresourceRange.layerCount = 1;
            b.srcAccessMask = 0;
            b.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            vkCmdPipelineBarrier(cmd,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                            VK_PIPELINE_STAGE_TRANSFER_BIT,
                            0, 0, nullptr, 0, nullptr, 1, &b);
        }

        {
            VkBufferImageCopy region{};
            region.bufferOffset = 0;
            region.bufferRowLength = 0;
            region.bufferImageHeight = 0;
            region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = 1;
            region.imageOffset = {0, 0, 0};
            region.imageExtent = {(uint32_t)w, (uint32_t)h, 1};

            vkCmdCopyBufferToImage(cmd, stagingBuffer, image,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                1, &region);
        }

        {
            VkImageMemoryBarrier b{};
            b.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            b.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            b.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            b.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            b.image = image;
            b.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            b.subresourceRange.baseMipLevel = 0;
            b.subresourceRange.levelCount = 1;
            b.subresourceRange.baseArrayLayer = 0;
            b.subresourceRange.layerCount = 1;
            b.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            b.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(cmd,
                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                0, 0, nullptr, 0, nullptr, 1, &b);
        }

        vkEndCommandBuffer(cmd);

        VkSubmitInfo si{};
        si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        si.commandBufferCount = 1;
        si.pCommandBuffers = &cmd;
        vkQueueSubmit(graphicsQueue, 1, &si, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);

        vkFreeCommandBuffers(device, commandPool, 1, &cmd);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);
    }

    VkImageView view = VK_NULL_HANDLE;
    {
        VkImageViewCreateInfo vi{};
        vi.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        vi.image = image;
        vi.viewType = VK_IMAGE_VIEW_TYPE_2D;
        vi.format = VK_FORMAT_R8G8B8A8_SRGB;
        vi.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vi.subresourceRange.baseMipLevel = 0;
        vi.subresourceRange.levelCount = 1;
        vi.subresourceRange.baseArrayLayer = 0;
        vi.subresourceRange.layerCount = 1;
        if(vkCreateImageView(device, &vi, nullptr, &view) != VK_SUCCESS) {
            vkDestroyImage(device, image, nullptr);
            vkFreeMemory(device, imageMemory, nullptr);
            throw std::runtime_error("TextureLoader: failed to create image view");
        }
    }

    VkSampler sampler = VK_NULL_HANDLE;
    {
        VkSamplerCreateInfo si{};
        si.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        si.magFilter = VK_FILTER_LINEAR;
        si.minFilter = VK_FILTER_LINEAR;
        si.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        si.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        si.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        si.anisotropyEnable = VK_FALSE;
        si.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        si.unnormalizedCoordinates = VK_FALSE;
        si.compareEnable = VK_FALSE;
        si.compareOp = VK_COMPARE_OP_ALWAYS;
        si.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        si.minLod = 0.0f;
        si.maxLod = 0.0f;
        if(vkCreateSampler(device, &si, nullptr, &sampler) != VK_SUCCESS) {
            vkDestroyImageView(device, view, nullptr);
            vkDestroyImage(device, image, nullptr);
            vkFreeMemory(device, imageMemory, nullptr);
            throw std::runtime_error("TextureLoader: failed to create sampler");
        }
    }

    const uint32_t texId = static_cast<uint32_t>(textures.size());
    Texture t;
    t.image = image;
    t.memory = imageMemory;
    t.view = view;
    t.slot = slot;
    t.sampler = sampler;
    textures[texId] = t;
    toTexId[id] = texId;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = view;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstBinding = 0;
    write.dstArrayElement = slot;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;

    for(VkDescriptorSet ds : descriptorSets) {
        write.dstSet = ds;
        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    return texId;
}

/**
 * 
 * Init
 * 
 */
void TextureLoader::init(uint32_t frames) {
    this->device = main.device;
    this->physicalDevice = main.physicalDevice;
    this->commandPool = main.commandPool;
    this->graphicsQueue = main.graphicsQueue;

    VkPhysicalDeviceDescriptorIndexingProperties idxProps{};
    idxProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;

    VkPhysicalDeviceProperties2 props2{};
    props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    props2.pNext = &idxProps;
    vkGetPhysicalDeviceProperties2(physicalDevice, &props2);

    maxTextures = idxProps.maxPerStageDescriptorUpdateAfterBindSampledImages;
    if(maxTextures == 0) {
        throw std::runtime_error(
            "TextureLoader: device does not support update-after-bind sampled images "
            "(VK_EXT_descriptor_indexing required)");
    }

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = maxTextures;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorBindingFlags flags =
                            VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                            VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
    bindingFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    bindingFlags.bindingCount = 1;
    bindingFlags.pBindingFlags = &flags;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pNext = &bindingFlags;
    layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("TextureLoader: failed to create descriptor set layout");
    }

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = maxTextures;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = frames;

    if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("TextureLoader: failed to create descriptor pool");
    }

    std::vector<VkDescriptorSetLayout> layouts(frames, descriptorSetLayout);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = frames;
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(frames);
    if(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("TextureLoader: failed to allocate descriptor sets");
    }
}

/**
 * Cleanup
 */
void TextureLoader::cleanup() {
    if(device == VK_NULL_HANDLE) return;

    for(auto& [id, t] : textures) {
        if(t.sampler != VK_NULL_HANDLE) vkDestroySampler(device, t.sampler, nullptr);
        if(t.view != VK_NULL_HANDLE) vkDestroyImageView(device, t.view, nullptr);
        if(t.image != VK_NULL_HANDLE) vkDestroyImage(device, t.image, nullptr);
        if(t.memory != VK_NULL_HANDLE) vkFreeMemory(device, t.memory, nullptr);
    }
    textures.clear();
    toTexId.clear();
    
    if(descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }
    if(descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
        descriptorSetLayout = VK_NULL_HANDLE;
    }
    descriptorSets.clear();

    nextSlot = 0;
    maxTextures = 0;
    device = VK_NULL_HANDLE;
}