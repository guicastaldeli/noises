#pragma once

#include <cake.hpp>
#include <window.hpp>
#include <imgui.h>
#include <data.hpp>
#include <chunk.hpp> 
#include <vulkan/vulkan.h>
#include <utils/enum.hpp>
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
    /*** Render ***/template<typename T>void R(VkCommandBuffer cmd, T& data) {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Noise
        ImGui::Begin("Noise");
        {
            combo("Type", data.noise.type);
            combo("Interp", data.noise.interp);
            combo("Fractal", data.noise.fractal);
            combo("Cell Dist", data.noise.cellDist);
            combo("Cell Ret", data.noise.cellRet);

            ImGui::Separator();
            ImGui::SliderFloat("Frequency", &data.noise.frequency, 0.001f, 0.2);
            ImGui::SliderInt("Seed", &data.noise.seed, 0, 100000);
            ImGui::SliderInt("Octaves", &data.noise.octaves, 1, 10);
            ImGui::SliderFloat("Lacunary", &data.noise.lacunary, 1.0f, 4.0f);
            ImGui::SliderFloat("Gain", &data.noise.gain, 0.0f, 1.0f);

            const float maxY = float(data.chunk.sizeY);
            ImGui::SliderFloat("Min Height", &data.noise.minHeight, 0.0f, maxY);
            ImGui::SliderFloat("Max Height", &data.noise.maxHeight, 0.0f, maxY);
        }
        ImGui::End();

        // Chunk
        ImGui::Begin("Chunk");
        {
            combo("Type", data.chunk.type);

            ImGui::SliderInt("Size X", &data.chunk.sizeX, 1, MAX_CHUNK_X_SIZE);
            ImGui::SliderInt("Size Y", &data.chunk.sizeY, 1, MAX_CHUNK_Y_SIZE);
            ImGui::SliderInt("Size Z", &data.chunk.sizeZ, 1, MAX_CHUNK_Z_SIZE);

            ImGui::Separator();

            if(data.chunk.type == ChunkType::Wave) {
                ImGui::SliderFloat("Wave Amplitude", &data.chunk.waveAmplitude, 0.0f, 16.0f);
                ImGui::SliderFloat("Wave Frequency", &data.chunk.waveFrequency, 0.0f, 4.0f);
            }
            if(data.chunk.type == ChunkType::Cheese) {
                ImGui::SliderFloat("Cheese Density", &data.chunk.cheeseDensity, 0.0f, 1.0f);
            }
            if(data.chunk.type == ChunkType::Cave) {
                ImGui::SliderFloat("Cave Threshold", &data.chunk.caveThreshold, 0.0f, 1.0f);
            }

            ImGui::Separator();

            ImGui::Checkbox("Collision", &data.collision);

            if(ImGui::Button("Load Terrain")) data.loadTerrain();
        }
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