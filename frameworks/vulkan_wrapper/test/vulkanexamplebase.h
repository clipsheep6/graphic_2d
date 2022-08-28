/*
* Vulkan Example base class
*
* Copyright (C) by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <cassert>
#include <vector>
#include <array>
#include <unordered_map>
#include <numeric>
#include <ctime>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>
#include <sys/stat.h>

#include <string>
#include <numeric>
#include <array>

#include "VulkanUtils.h"

#include "vulkan/vulkan.h"

#include "VulkanSwapChain.h"
#include "VulkanBuffer.h"
#include "VulkanDevice.h"

#include "VulkanInitializers.hpp"

#define DEFAULT_FENCE_TIMEOUT 100000000000
class VulkanExampleBase
{
private:
    std::string getWindowTitle();
    bool viewUpdated = false;
    uint32_t destWidth;
    uint32_t destHeight;
    bool resizing = false;
    void windowResize();
    void handleMouseMove(int32_t x, int32_t y);
    void nextFrame();
    void createPipelineCache();
    void createCommandPool();
    void createSynchronizationPrimitives();
    void initSwapchain();
    void setupSwapChain();
    void createCommandBuffers();
    void destroyCommandBuffers();
    std::string shaderDir = "glsl";
    bool libLoaded = false;
protected:
    // Frame counter to display fps
    uint32_t frameCounter = 0;
    uint32_t lastFPS = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp;
    // Vulkan instance, stores all per-application states
    VkInstance instance;
    std::vector<std::string> supportedInstanceExtensions;
    // Physical device (GPU) that Vulkan will use
    VkPhysicalDevice physicalDevice;
    // Stores physical device properties (for e.g. checking device limits)
    VkPhysicalDeviceProperties deviceProperties;
    // Stores the features available on the selected physical device (for e.g. checking if a feature is available)
    VkPhysicalDeviceFeatures deviceFeatures;
    // Stores all available memory (type) properties for the physical device
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    VkPhysicalDeviceFeatures enabledFeatures{};
    std::vector<const char*> enabledDeviceExtensions;
    std::vector<const char*> enabledInstanceExtensions;
    void* deviceCreatepNextChain = nullptr;
    VkDevice device;
    // Handle to the device graphics queue that command buffers are submitted to
    VkQueue queue;
    // Depth buffer format (selected during Vulkan initialization)
    VkFormat depthFormat;
    // Command buffer pool
    VkCommandPool cmdPool;
    VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    // Contains command buffers and semaphores to be presented to the queue
    VkSubmitInfo submitInfo;
    // Command buffers used for rendering
    std::vector<VkCommandBuffer> drawCmdBuffers;
    // Global render pass for frame buffer writes
    VkRenderPass renderPass = VK_NULL_HANDLE;
    // List of available frame buffers (same as number of swap chain images)
    std::vector<VkFramebuffer>frameBuffers;
    // Active frame buffer index
    uint32_t currentBuffer = 0;
    // Descriptor set pool
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    // List of shader modules created (stored for cleanup)
    std::vector<VkShaderModule> shaderModules;
    // Pipeline cache object
    VkPipelineCache pipelineCache;
    // Wraps the swap chain to present images (framebuffers) to the windowing system
    VulkanSwapChain swapChain;
    // Synchronization semaphores
    struct {
        // Swap chain image presentation
        VkSemaphore presentComplete;
        // Command buffer submission and execution
        VkSemaphore renderComplete;
    } semaphores;
    std::vector<VkFence> waitFences;
public:
    bool prepared = false;
    bool resized = false;
    uint32_t width = 1280;
    uint32_t height = 720;

    float frameTimer = 1.0f;

    vks::VulkanDevice *vulkanDevice;

    VkClearColorValue defaultClearColor = { { 0.025f, 0.025f, 0.025f, 1.0f } };

    static std::vector<const char*> args;

    // Defines a frame rate independent timer value clamped from -1.0...1.0
    // For use in animations, rotations, etc.
    float timer = 0.0f;
    // Multiplier for speeding up (or slowing down) the global timer
    float timerSpeed = 0.25f;
    bool paused = false;

    std::string name = "vulkanExample";
    uint32_t apiVersion = VK_API_VERSION_1_0;
    NativeWindow* window = nullptr;


    struct {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depthStencil;

    VulkanExampleBase();
    virtual ~VulkanExampleBase();
    bool initVulkan();


    void setupWindow(NativeWindow* nativeWindow)
    {
        window = nativeWindow;
    }

    virtual VkResult createInstance();
    virtual void render() = 0;
    virtual void viewChanged();
    virtual void keyPressed(uint32_t);
    virtual void windowResized();
    virtual void buildCommandBuffers();
    virtual void setupDepthStencil();
    virtual void setupFrameBuffer();
    virtual void setupRenderPass();

    virtual void prepare();

    VkPipelineShaderStageCreateInfo loadShader(std::string fileName, VkShaderStageFlagBits stage);

    void renderLoop();

    void drawUI(const VkCommandBuffer commandBuffer);

    void prepareFrame();
    void submitFrame();
    virtual void renderFrame();
};