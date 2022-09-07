/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "driver_loader.h"
#include <dlfcn.h>
#include <iostream>
#include "wrapper_log.h"
#define LIB_CACULATE_PATH "/system/lib64/libGLES_mali_vk.so"
#define HDI_VULKAN_MODULE_INIT "VulkanInitialize"
#define HDI_VULKAN_MODULE_UNINIT "VulkanUnInitialize"
namespace vulkan {
namespace driver {

bool DriverLoader::Load()
{
    std::cout << "DriverLoader::Load is comming" << std::endl;
    if (loader_.vulkanFuncs_ != nullptr) {
        return true;
    }

    void* handle = dlopen(LIB_CACULATE_PATH, RTLD_LOCAL | RTLD_NOW);

    if (handle == nullptr) {
        std::cout << "DriverLoader:: dlopen faild : " << dlerror() << std::endl;
        return false;
    }
    std::cout << "DriverLoader::dlopen "<< LIB_CACULATE_PATH <<" success" << std::endl;

    PFN_VulkanInitialize VulkanInitializeFunc
        = reinterpret_cast<PFN_VulkanInitialize>(dlsym(handle, HDI_VULKAN_MODULE_INIT));

    if (VulkanInitializeFunc == nullptr) {
        std::cout << "DriverLoader:: couldn't find symbol(" 
            << HDI_VULKAN_MODULE_INIT <<") in library : " << dlerror() << std::endl;
        dlclose(handle);
        return false;
    }

    if (VulkanInitializeFunc(&loader_.vulkanFuncs_) != 0) {
        std::cout << "Initialize Vulkan Func fail" << std::endl;
        return false;
    }


    loader_.vulkanUnInitializeFunc_ = reinterpret_cast<PFN_VulkanUnInitialize>(dlsym(handle, HDI_VULKAN_MODULE_UNINIT));

    if (loader_.vulkanUnInitializeFunc_ == nullptr) {
        std::cout << "DriverLoader:: couldn't find symbol(" 
            << HDI_VULKAN_MODULE_UNINIT <<") in library : " << dlerror() << std::endl;
        dlclose(handle);
        return false;
    }
   
    std::cout << "open success" << std::endl;

    return true;
}

bool DriverLoader::Unload()
{
    if (!loader_.vulkanUnInitializeFunc_) {
        std::cout << "DriverLoader::Unload can not find vulkan UnInitialize Func " << std::endl;
        return false;
    }
    if (loader_.vulkanUnInitializeFunc_(loader_.vulkanFuncs_) != 0) {
        std::cout << "DriverLoader::Unload vulkan UnInitialize Func success" << std::endl;
        return false;
    }
    return true;
}


}  // namespace driver
}  // namespace vulkan
