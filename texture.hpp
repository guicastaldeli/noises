#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

class Main;
class TextureLoader {
private:
    struct Texture {
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        uint32_t slot = 0;
    };

    Main& main;

    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> descriptorSets;

    std::unordered_map<uint32_t, uint32_t> toTexId;
    std::unordered_map<uint32_t, Texture> textures;

    uint32_t maxTextures = 0;
    uint32_t nextSlot = 0;

public:
    TextureLoader(Main& main);
    ~TextureLoader();

    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    VkDescriptorSet getDescriptorSet(uint32_t frame) const { return descriptorSets[frame]; }
    
    uint32_t load(uint32_t id, const std::string& path);
    void init(uint32_t frames);

    void cleanup();
};