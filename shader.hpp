#pragma once

#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

struct ShaderStage {
    VkShaderStageFlagBits   stage;
    const char*             path;
    const char*             entry = "main";
};

class Main;
class Shader {
private:
    /****
     *
     *  Shader Stage
     * 
     **/
    inline static const ShaderStage Src[] = {
        { VK_SHADER_STAGE_VERTEX_BIT,   "vert.spv" },
        { VK_SHADER_STAGE_FRAGMENT_BIT, "frag.spv" }
    };
    /**
     * 
     */

    std::vector<VkPipelineShaderStageCreateInfo> stageInfos;
    std::vector<VkShaderModule> modules;

    Main& main;

    void set();
    
public:
    Shader(Main& main);
    ~Shader();

    static std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& src);

    const std::vector<VkPipelineShaderStageCreateInfo>& getStageInfos() const;

    void run();
    void cleanup();

    /***********
     * Vertex Input
     *****/
    VkPipelineVertexInputStateCreateInfo vertexInputInfo() {
        VkPipelineVertexInputStateCreateInfo i{};
        i.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        i.vertexBindingDescriptionCount = 0;
        i.vertexAttributeDescriptionCount = 0;
    }
};