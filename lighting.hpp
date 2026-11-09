#pragma once

#include <utils/vec3.hpp>
#include <utils/color.hpp>
#include <utils/idgen.hpp>
#include <string>
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include <shader.hpp>
#include <unordered_map>

#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048

struct UniformBuffer;

/********************
 * 
 * Light Data
 * 
 */
struct LightData {
    float position[3];  float padding1;
    float color[3];     float intensity;
};

struct DirLightData {
    float direction[3]; float padding1;
    float color[3];     float intensity;
};

struct Lights {
    float ambientColor[3];
    float ambientIntensity;

    int pointCount;
    int dirCount;
    float padding1[2];

    LightData pointLights[32];
    DirLightData dirLights[4];

    Mat4 lightSpaceMatrix;
    int shadowMapWidth;
    int shadowMapHeight;
    float padding2[2];
};
/**
 * 
 */

class Main;
class Mesh;

/************************
 * 
 * Light Type
 * 
 */
class LightingManager;
enum class LightType {
    AMBIENT,
    POINT,
    DIRECTIONAL
};

/*****************
 * 
 * Lighting
 * 
 *  ******************/
class Lighting {
protected:
    Vec3 color{ 1.0f, 1.0f, 1.0f };
    float intensity = 1.0f;

public:
    Lighting(LightingManager& lightingManager);
    virtual ~Lighting() = default;

    LightingManager& lightingManager;

    virtual uint32_t getId() const { return 0; }
    virtual LightType getType() const = 0;
    virtual void setColor(float r, float g, float b);
    virtual void setColor(const std::string& hex);
    virtual void setIntensity(float i);

    virtual void upload(UniformBuffer& ubo) const = 0;

    Vec3 getColor() const { return color; }
    float getIntensity() const { return intensity; }
};
/**
 * 
 */

// ---------------------------------------
//
// Lights
//
// ---------------------------------------
/******************************
 * Ambient Light
 ****************/
class AmbientLight : public Lighting {
public:
    AmbientLight(LightingManager& lightingManager);
    LightType getType() const override;

    void upload(UniformBuffer& ubo) const override;
};

/****************
 * Point Light
 ****************/
class PointLight : public Lighting {
private:
    uint32_t id;
    Vec3 position{ 0.0f, 0.0f, 0.0f };
    float radius = 1.0f;

public:
    PointLight(LightingManager& lightingManager);
    
    LightType getType() const override;
    uint32_t getId() const override;
    void setPosition(float x, float y, float z);
    void setRadius(float r);
    Vec3 getPosition() const;
    float getRadius() const;

    void upload(UniformBuffer& ubo) const override;
    static void uploadCount(UniformBuffer& ubo, int count);
};

/********************
 * Directional Light 
 * *******/
class DirectionalLight : public Lighting {
private:
    uint32_t id;
    Vec3 direction{ -0.5f, -1.0f, -0.3f };

public:
    DirectionalLight(LightingManager& lightingManager);

    Mat4 getLightSpaceMatrix(const Vec3& sceneCenter, float sceneRadius) const;

    LightType getType() const override;
    uint32_t getId() const override;
    void setDirection(float x, float y, float z);
    Vec3 getDirection() const;

    void upload(UniformBuffer& ubo) const override;
    static void uploadCount(UniformBuffer& ubo, int count);
};
// ---------------------------------------

/***************************
 * 
 * Lighting Manager
 * 
 */
class LightingManager {
private:
    Main& main;
    Shader& shader;
    Mesh& mesh;

    std::vector<std::unique_ptr<Lighting>> lights;
    Lighting* current = nullptr;

    AmbientLight* ambientLight = nullptr;
    std::vector<PointLight*> pointLight;
    std::vector<DirectionalLight*> directionalLight;

public:
    LightingManager(Main& main, Shader& shader, Mesh& mesh);
    ~LightingManager();

    LightingManager& add(LightType type);
    void upload(UniformBuffer& ubo);

    const Mesh& getMesh() const;

    LightingManager& setColor(float r, float g, float b);
    LightingManager& setColor(const std::string& hex);
    LightingManager& setIntensity(float i);
    LightingManager& setPosition(float x, float y, float z);
    LightingManager& setDirection(float x, float y, float z);

    void init();
    void create();

    void clear();

    AmbientLight* getAmbientLight() const { return ambientLight; }
    const std::vector<PointLight*>& getPointLight() const { return pointLight; }
    const std::vector<DirectionalLight*>& getDirectionalLight() const { return directionalLight; }

    /***************************
     * 
     * Shadow Map
     * 
     ****************/
    struct ShadowMap {
        static inline VkFormat shadowFormat = VK_FORMAT_D32_SFLOAT;
        static inline VkImage shadowImage = VK_NULL_HANDLE;
        static inline VkDeviceMemory shadowImageMemory = VK_NULL_HANDLE;
        static inline VkImageView shadowImageView = VK_NULL_HANDLE;
        static inline VkFramebuffer shadowFramebuffer = VK_NULL_HANDLE;
        static inline VkRenderPass shadowRenderPass = VK_NULL_HANDLE;

        static inline VkSampler shadowSampler = VK_NULL_HANDLE;

        static inline VkDescriptorSetLayout shadowDescriptorSetLayout = VK_NULL_HANDLE;
        static inline VkDescriptorPool shadowDescriptorPool = VK_NULL_HANDLE;
        static inline VkDescriptorSet shadowDescriptorSet = VK_NULL_HANDLE;

        static inline VkPipeline shadowPipeline = VK_NULL_HANDLE;
        static inline VkPipelineLayout shadowPipelineLayout = VK_NULL_HANDLE;
        
        /****
         * Create Shadow Resources
         */
        static void createShadowResources(Main main) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = SHADOW_MAP_WIDTH;
            imageInfo.extent.height = SHADOW_MAP_HEIGHT;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = shadowFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            if(vkCreateImage(main.device, &imageInfo, nullptr, &shadowImage) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow image!!");
            }

            VkMemoryRequirements memReq;
            vkGetImageMemoryRequirements(main.device, shadowImage, &memReq);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memReq.size;
            allocInfo.memoryTypeIndex = [&]() -> uint32_t {
                VkPhysicalDeviceMemoryProperties props;
                vkGetPhysicalDeviceMemoryProperties(main.physicalDevice, &props);
                for(uint32_t i = 0; i < props.memoryTypeCount; i++) {
                    if((memReq.memoryTypeBits & (1 << i)) &&
                        (props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                        return i;
                    }
                }

                throw std::runtime_error("no suitable memory for shadow image");
            }();

            if(vkAllocateMemory(main.device, &allocInfo, nullptr, &shadowImageMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate shadow image memory!");
            }
            vkBindImageMemory(main.device, shadowImage, shadowImageMemory, 0);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = shadowImage;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = shadowFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if(vkCreateImageView(main.device, &viewInfo, nullptr, &shadowImageView) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow image view!");
            }
        }

        /**
         * Create Shadow Render Pass
         */
        static void createShadowRenderPass(VkDevice device) {
            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = shadowFormat;
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 0;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 0;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments = &depthAttachment;
            renderPassInfo.subpassCount = 1;
            renderPassInfo.pSubpasses = &subpass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies = &dependency;

            if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &shadowRenderPass) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow render pass!");
            }
        }

        /**
         * Create Shadow Framebuffer
         */
        static void createShadowFramebuffer(VkDevice device) {
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = shadowRenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = &shadowImageView;
            framebufferInfo.width = SHADOW_MAP_WIDTH;
            framebufferInfo.height = SHADOW_MAP_HEIGHT;
            framebufferInfo.layers = 1;

            if(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &shadowFramebuffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow framebuffer");
            }
        }

        /**
         * Create Shadow Sampler
         */
        static void createShadowSampler(VkDevice device) {
            VkSamplerCreateInfo samplerInfo{};
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            samplerInfo.compareEnable = VK_TRUE;
            samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

            if(vkCreateSampler(device, &samplerInfo, nullptr, &shadowSampler) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow sampler!");
            }
        }

        /**
         * Create Shadow Descriptor
         */
        static void createShadowDescriptor(VkDevice device) {
            VkDescriptorSetLayoutBinding binding{};
            binding.binding = 0;
            binding.descriptorCount = 1;
            binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = 1;
            layoutInfo.pBindings = &binding;

            if(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &shadowDescriptorSetLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow descriptor set layout!");
            }

            VkDescriptorPoolSize poolSize{};
            poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSize.descriptorCount = 1;

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.pPoolSizes = &poolSize;
            poolInfo.maxSets = 1;

            if(vkCreateDescriptorPool(device, &poolInfo, nullptr, &shadowDescriptorPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow descriptor pool!");
            }

            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = shadowDescriptorPool;
            allocInfo.descriptorSetCount = 1;
            allocInfo.pSetLayouts = &shadowDescriptorSetLayout;

            if(vkAllocateDescriptorSets(device, &allocInfo, &shadowDescriptorSet) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate shadow descriptor set!");
            }

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = shadowImageView;
            imageInfo.sampler = shadowSampler;

            VkWriteDescriptorSet write{};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.dstSet  =shadowDescriptorSet;
            write.dstBinding = 0;
            write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            write.descriptorCount = 1;
            write.pImageInfo = &imageInfo;

            vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
        }

        /**
         * Create Shadow Pipeline
         */
        static void createShadowPipeline(Main main, Shader shader, Mesh mesh) {
            VkPipelineShaderStageCreateInfo shadowStage{};
            const auto& stages = shader.getStageInfos();
            shadowStage = stages[2];

            VkPipelineVertexInputStateCreateInfo vertexInputInfo = mesh.getVertexInputState();

            VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineDepthStencilStateCreateInfo depthStencil{};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

            VkPipelineColorBlendStateCreateInfo colorBlending{};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.attachmentCount = 0;

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(main.dynamicStates.size());
            dynamicState.pDynamicStates = main.dynamicStates.data();

            VkDescriptorSetLayout layouts[] = { mesh.getDescriptorSetLayout() };
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = layouts;
            if(vkCreatePipelineLayout(main.device, &pipelineLayoutInfo, nullptr, &shadowPipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow pipeline layout!");
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 1;
            pipelineInfo.pStages = &shadowStage;
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = &dynamicState;
            pipelineInfo.layout = shadowPipelineLayout;
            pipelineInfo.renderPass = shadowRenderPass;
            pipelineInfo.subpass = 0;

            if(vkCreateGraphicsPipelines(main.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shadowPipeline) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shadow graphics pipeline...");
            }
        }

        /**
         * 
         * Init
         * 
         */
        static void init(Main main) {
            createShadowResources(main);
            createShadowRenderPass(main.device);
            createShadowFramebuffer(main.device);
            createShadowSampler(main.device);
            createShadowDescriptor(main.device);
        }

        /**
         * 
         * Create
         * 
         */
        static void create(Main main, Shader shader, Mesh mesh) {
            createShadowPipeline(main, shader, mesh);
        }

        /**
         * 
         * Render
         * 
         */
        static void render(VkCommandBuffer cmd, Mesh& mesh) {
            VkRenderPassBeginInfo rp{};
            rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            rp.renderPass = shadowRenderPass;
            rp.framebuffer = shadowFramebuffer;
            rp.renderArea.offset = {0, 0};
            rp.renderArea.extent = {SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT};

            VkClearValue clear{};
            clear.depthStencil = {1.0f, 0};
            rp.clearValueCount = 1;
            rp.pClearValues = &clear;

            vkCmdBeginRenderPass(cmd, &rp, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline);

            VkViewport vp{};
            vp.width = SHADOW_MAP_WIDTH;
            vp.height = SHADOW_MAP_HEIGHT;
            vp.minDepth = 0.0f;
            vp.maxDepth = 1.0f;
            vkCmdSetViewport(cmd, 0, 1, &vp);

            VkRect2D sc{};
            sc.extent = {SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT};
            vkCmdSetScissor(cmd, 0, 1, &sc);

            mesh.render(true);
            vkCmdEndRenderPass(cmd);
        }

        /**
         * Clear
         */
        static void clear(VkDevice device) {
            vkDestroyDescriptorPool(device, shadowDescriptorPool, nullptr);
            vkDestroyDescriptorSetLayout(device, shadowDescriptorSetLayout, nullptr);
            vkDestroySampler(device, shadowSampler, nullptr);
            vkDestroyFramebuffer(device, shadowFramebuffer, nullptr);
            vkDestroyRenderPass(device, shadowRenderPass, nullptr);
            vkDestroyImageView(device, shadowImageView, nullptr);
            vkDestroyImage(device, shadowImage, nullptr);
            vkFreeMemory(device, shadowImageMemory, nullptr);
        }
    };
/**
 * 
 */
};
/**
 * 
 */