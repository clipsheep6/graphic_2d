/*
* Assorted Vulkan helper functions
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#pragma once

#include "vulkan/vulkan.h"
#include "VulkanInitializers.hpp"

#include <cmath>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include "VulkanUtils.h"

// Custom define for better code readability
#define VK_FLAGS_NONE 0
// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

// Macro to check and display Vulkan return results

#define VK_CHECK_RESULT(f) do {                                                                           \
    VkResult res = (f);                                                                                    \
    if (res != VK_SUCCESS)                                                                                \
    {                                                                                                    \
        std::cout << "Fatal : VkResult is " \
			<< vks::tools::errorString(res).c_str() << " in " <<  __FILE__ << "at line " <<  __LINE__; \
        assert(res == VK_SUCCESS);                                                                        \
    }                                                                                                    \
} while (0)


namespace vks
{
namespace tools
{

std::string errorString(VkResult errorCode);

std::string physicalDeviceTypeString(VkPhysicalDeviceType type);

// Selected a suitable supported depth format starting with 32 bit down to 16 bit
// Returns false if none of the depth formats in the list is supported by the device
VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);

// Returns tru a given format support LINEAR filtering
VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling);
// Returns true if a given format has a stencil part
VkBool32 formatHasStencil(VkFormat format);

// Display error message and exit on fatal error
void exitFatal(const std::string& message, int32_t exitCode);
void exitFatal(const std::string& message, VkResult resultCode);

// Load a SPIR-V shader (binary)
VkShaderModule loadShader(const char *fileName, VkDevice device);

bool fileExists(const std::string &filename);

uint32_t alignedSize(uint32_t value, uint32_t alignment);
} // namespace tools
} // namespace vks
