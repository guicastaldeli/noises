#include <shader.hpp>
#include <fstream>
#include <cake.hpp>

Shader::Shader(Main& main) : main(main) {};
Shader::~Shader() {};

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
 * Init
 * 
 */
void Shader::init() {
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