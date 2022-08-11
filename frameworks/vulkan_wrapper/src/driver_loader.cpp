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

    void* handle = nullptr;
    handle = dlopen(LIB_CACULATE_PATH, RTLD_LOCAL | RTLD_NOW);

    if (handle == nullptr) {
        std::cout << "DriverLoader:: dlopen faild : " << dlerror() << std::endl;
        return false;
    }
    std::cout << "DriverLoader::dlopen "<< LIB_CACULATE_PATH <<" success" << std::endl;

    PFN_VulkanInitialize VulkanInitializeFunc = reinterpret_cast<PFN_VulkanInitialize>(dlsym(handle, HDI_VULKAN_MODULE_INIT));

    if (VulkanInitializeFunc == nullptr) {
        std::cout << "DriverLoader:: couldn't find symbol(" << HDI_VULKAN_MODULE_INIT <<") in library : " << dlerror() << std::endl;
        dlclose(handle);
        return false;
    }

    if (VulkanInitializeFunc(&loader_.vulkanFuncs_) != 0) {
        std::cout << "Initialize Vulkan Func fail" << std::endl;
        return false;
    }


    loader_.vulkanUnInitializeFunc_ = reinterpret_cast<PFN_VulkanUnInitialize>(dlsym(handle, HDI_VULKAN_MODULE_UNINIT));

    if (loader_.vulkanUnInitializeFunc_ == nullptr) {
        std::cout << "DriverLoader:: couldn't find symbol(" << HDI_VULKAN_MODULE_UNINIT <<") in library : " << dlerror() << std::endl;
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
