#include <cake.hpp>
#include <iostream>
#include <window.hpp>

Main::Main() {
    window  = new Window(*this);
    shader  = new Shader(*this);
    world   = new World(*this);
}
Main::~Main() {
    delete shader;  shader = nullptr;
    delete window;  window = nullptr;
    delete world;   world = nullptr;
}

/**
 * Set
 */
void Main::Set() {
    window->set();
}

/**
 * Update
 */
void Main::Update() {
    window->update();
    world->update();
}

/**
 * Create
 */
void Main::Create() {
    auto stageInfos = shader->getStageInfos();
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = world->getMesh()->getVertexInputState();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
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

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(stageInfos.size());
    pipelineInfo.pStages = stageInfos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
}

/**
 * 
 * Render
 * 
 */
void Main::Render() {
    world->render();   
}

void Main::SetRenderPass(VkCommandBuffer cmd, uint32_t imageIndex) {
    VkRenderPassBeginInfo rp{};
    rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp.renderPass = renderPass;
    rp.framebuffer = swapChainFramebuffers[imageIndex];
    rp.renderArea.offset = {0, 0};
    rp.renderArea.extent = swapChainExtent;

    VkClearValue clear = {{{0.5, 0.5, 0.5, 1.0f}}};
    rp.clearValueCount = 1;
    rp.pClearValues = &clear;

    vkCmdBeginRenderPass(cmd, &rp, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport vp{};
        vp.width = (float)swapChainExtent.width;
        vp.height = (float)swapChainExtent.height;
        vp.maxDepth = 1.0f;
        vkCmdSetViewport(cmd, 0, 1, &vp);

        VkRect2D sc{};
        sc.extent = swapChainExtent;
        vkCmdSetScissor(cmd, 0, 1, &sc);

        Render();
    vkCmdEndRenderPass(cmd);
}

/**
 * 
 * Init
 * 
 */
void Main::SetInit() {
    window->init();
}

void Main::Init() {
    world->init();
}

/**
 * 
 * Run
 * 
 */
void Main::Run() {
    SetInit();
    Set();

    RunPipeline();

    Init();
    shader->run();

    Create();
    Update();

    Cleanup();
}

/**
 * Cleanup
 */
void Main::Cleanup() {
    if(device != VK_NULL_HANDLE) vkDeviceWaitIdle(device);

    cleanupSwapChain();

    if(commandPool != VK_NULL_HANDLE) vkDestroyCommandPool(device, commandPool, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, inFlightFences[i], nullptr);
    }
    for(size_t i = 0; i < renderFinishedSemaphores.size(); i++) {
        vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
    }

    if(graphicsPipeline != VK_NULL_HANDLE) vkDestroyPipeline(device, graphicsPipeline, nullptr);
    if(pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    if(renderPass != VK_NULL_HANDLE) vkDestroyRenderPass(device, renderPass, nullptr);
    
    shader->cleanup();
    world->cleanup();

    if(device != VK_NULL_HANDLE) vkDestroyDevice(device, nullptr);
    if(window->surface != VK_NULL_HANDLE) vkDestroySurfaceKHR(instance, window->surface, nullptr);
    if(enableValitationLayers) DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    
    if(instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
    window->cleanup();
}

/******************
 * 
 * 
 * Main entry point and
 * initialization...
 * 
 * 
 ***********/
int main() {
    try {
        Main app;
        app.Run();
    } catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}