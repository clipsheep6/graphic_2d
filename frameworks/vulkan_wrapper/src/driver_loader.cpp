#include "driver_loader.h"
#include <dlfcn.h>
#include <iostream>
#include "wrapper_log.h"
#define LIB_CACULATE_PATH "/system/lib64/libGLES_mali.so"
#define HAL_MODULE_INFO_SYM_AS_STR "HMI"
namespace vulkan {
namespace driver {

bool DriverLoader::Load()
{
    WLOGD("Andrew:: DriverLoader::Load is comming");
    if (loader_.dev_ != nullptr) {
        return true;
    }

    void* handle = nullptr;
    handle = dlopen(LIB_CACULATE_PATH, RTLD_LOCAL | RTLD_NOW);

    if (handle == nullptr) {
        std::cout << "dlopen faild : " << dlerror() << std::endl;
        return false;
    }
    std::cout << "dlopen success" << std::endl;

    auto hmi = static_cast<hw_module_t*>(dlsym(handle, HAL_MODULE_INFO_SYM_AS_STR));

    if (hmi == nullptr) {
        std::cout << "couldn't find symbol(" << HAL_MODULE_INFO_SYM_AS_STR <<") in library : " << dlerror() << std::endl;
        dlclose(handle);
        return false;
    }

    std::cout << "dlsym success" << hmi->id << std::endl;
    hmi->dso = handle;

    hwvulkan_device_t* device;
    int result = hmi->methods->open(hmi, HWVULKAN_DEVICE_0, reinterpret_cast<hw_device_t**>(&device));
    if (result != 0) {
        std::cout << "open failed" << std::endl;
        dlclose(handle);
        return false;
    } 
    loader_.dev_ = device;
    std::cout << "open success" << std::endl;

    return true;
}


}  // namespace driver
}  // namespace vulkan
