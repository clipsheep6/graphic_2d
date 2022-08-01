/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
// #include "vulkan/vulkan_core.h"
#include "vulkan/vulkan.h"
#include "api_export.h"

void InitSurface(NativeWindow* window, VkSurfaceKHR& out_surface, VkInstance instance)
{
	VkOHOSSurfaceCreateInfoOpenHarmony surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_OHOS_SURFACE_CREATE_INFO_OPENHARMONY;
	surfaceCreateInfo.window = window;
	VkResult err = vkCreateOHOSSurfaceOpenHarmony(instance, &surfaceCreateInfo, NULL, &out_surface);

	if (err != VK_SUCCESS) {
		std::cout << "Could not create surface!" << std::endl;
	}

}


void CreatSwapchain(VkSwapchainKHR swapChain, VkSurfaceKHR surface)
{
	// VkSwapchainKHR oldSwapchain = swapChain;
	// Get physical device surface properties and formats
	// Get available present modes

	VkExtent2D swapchainExtent = {1080, 1080};

	// Get physical device surface properties and formats
	// VkSurfaceCapabilitiesKHR surfCaps;


	// Determine the number of images
	uint32_t desiredNumberOfSwapchainImages = 3;

	VkSwapchainCreateInfoKHR swapchainCI = {};	
	swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCI.surface = surface;
	swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
	swapchainCI.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;//colorFormat;
	swapchainCI.imageColorSpace =VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;// colorSpace;
	swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
	swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCI.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;//(VkSurfaceTransformFlagBitsKHR)preTransform;
	swapchainCI.imageArrayLayers = 1;
	swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCI.queueFamilyIndexCount = 0;
	swapchainCI.presentMode = VK_PRESENT_MODE_FIFO_KHR;//swapchainPresentMode;
	VkDevice device = nullptr;
	vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain);
}

int32_t main(int32_t argc, const char* argv[])
{
    std::string name = "vulkanExample";
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = name.c_str();
	appInfo.pEngineName = name.c_str();
	appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = NULL;
	instanceCreateInfo.pApplicationInfo = &appInfo;

    VkInstance instance;
    VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
    std::cout << "Andrew :: CreateInstance success : " << result << std::endl;



    return 0;
}
