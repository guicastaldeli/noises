#pragma once

#include <cake.hpp>
#include <window.hpp>
#include <vulkan/vulkan.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

class Main;
class UI {
private:
    Main& main;
    Window& window;

    VkDescriptorPool pool = VK_NULL_HANDLE;

public:
    UI(Main& main, Window& window) :  main(main), window(window) {};
    ~UI() {}
    /*** Pool ****/template<typename T> VkDescriptorPool P() {
        auto& m = static_cast<T&>(main);
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType =            VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags =            VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.poolSizeCount =    m.poolSizesCount;
        poolInfo.pPoolSizes =       m.poolSizes;
        poolInfo.maxSets =          m.poolSizesCount * 1000;
        VkDescriptorPool uiPool;
        vkCreateDescriptorPool(m.device, &poolInfo, nullptr, &uiPool);

        return uiPool;
    }
    /*** Init ***/template<typename T> void I() {
        auto& m = static_cast<T&>(main);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        ImGui_ImplGlfw_InitForVulkan(window.window, true);

        pool = P<T>();
    
        ImGui_ImplVulkan_InitInfo initInfo{};
        initInfo.Instance = m.instance;
        initInfo.PhysicalDevice = m.physicalDevice;
        initInfo.Device = m.device;
        initInfo.QueueFamily = m.findQueueFamilies(m.physicalDevice).graphicsFamily.value();
        initInfo.Queue = m.graphicsQueue;
        initInfo.PipelineCache = VK_NULL_HANDLE;
        initInfo.DescriptorPool = pool;
        initInfo.MinImageCount = m.MAX_FRAMES_IN_FLIGHT;
        initInfo.ImageCount = m.swapChainImages.size();
        initInfo.Allocator = nullptr;
        initInfo.CheckVkResultFn = nullptr;
    
        initInfo.PipelineInfoMain.RenderPass = m.renderPass;
        initInfo.PipelineInfoMain.Subpass = 0;
        initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    
        ImGui_ImplVulkan_Init(&initInfo);
    }
    /*** Render ***/void R(VkCommandBuffer cmd) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");
        ImGui::Text("Hello world");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }
    /*** Cleanup ***/template<typename T> void C() {
        auto& m = static_cast<T&>(main);

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        if(pool != VK_NULL_HANDLE) {
            vkDestroyDescriptorPool(m.device, pool, nullptr);
            pool = VK_NULL_HANDLE;
        }
    }
};