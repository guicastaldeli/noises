#include <shader.hpp>
#include <fstream>
#include <cake.hpp>
#include <cstdint>
#include <cstring>
#include <iostream>

Shader::Shader(Main& main) : main(main) {};
Shader::~Shader() {};

/**
 * Get Stage Infos
 */
const std::vector<VkPipelineShaderStageCreateInfo>& Shader::getStageInfos() {
    std::vector<VkPipelineShaderStageCreateInfo> val = stageInfos;
    return stageInfos;
}

/**
 * Read File
 */
std::vector<char> Shader::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if(!file.is_open()) throw std::runtime_error("failed to open file!");

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

/**
 * Create Shader Module
 */
VkShaderModule Shader::createShaderModule(const std::vector<char>& src) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = src.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(src.data());

    VkShaderModule shaderModule;
    if(vkCreateShaderModule(main.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

/**
 * Set
 */
void Shader::set() {
    stageInfos.reserve(std::size(Src));
    modules.reserve(std::size(Src));

    for(const auto& s : Src) {
        auto code = readFile(s.path);
        std::cout << "[shader] " << s.path
          << "  size=" << code.size()
          << " bytes  first words:";
        for (size_t i = 0; i < std::min<size_t>(4, code.size() / 4); ++i) {
            uint32_t w;
            std::memcpy(&w, code.data() + i * 4, 4);
            std::cout << " 0x" << std::hex << w << std::dec;
        }
        std::cout << std::endl;

        VkShaderModule mod = createShaderModule(code);
        if(mod == VK_NULL_HANDLE) {
            throw std::runtime_error(std::string("failed to create shader module: ") + s.path);
        }
        modules.push_back(mod);

        VkPipelineShaderStageCreateInfo info{};
        info.sType      = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.stage      = s.stage;
        info.module     = mod;
        info.pName      = s.entry;
        stageInfos.push_back(info);
    }

}

/**
 * 
 * Render
 * 
 */
void Shader::render() {
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasClamp = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                            VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT |
                                            VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
}

/**
 * 
 * Run
 * 
 */
void Shader::run() {
    set();
}

/**
 * Cleanup
 */
void Shader::cleanup() {
    for(VkShaderModule m : modules) {
        vkDestroyShaderModule(main.device, m, nullptr);
    }
    modules.clear();
}