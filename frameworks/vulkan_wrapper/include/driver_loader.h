#include "hardware/hdi_vulkan.h"
namespace vulkan {
namespace driver {

typedef int32_t (*PFN_VulkanInitialize)(VulkanFuncs **funcs);
typedef int32_t (*PFN_VulkanUnInitialize)(VulkanFuncs *funcs);
class DriverLoader {

public:
static bool Load();
static bool Unload();

static const DriverLoader& Get() { return loader_; }
static const VulkanFuncs& GetVulkanFuncs() { return *Get().vulkanFuncs_; }

private:
DriverLoader() : vulkanFuncs_(nullptr), vulkanUnInitializeFunc_(nullptr) {}
DriverLoader(const DriverLoader&) = delete;
DriverLoader& operator=(const DriverLoader&) = delete;

static DriverLoader loader_;

VulkanFuncs* vulkanFuncs_;
PFN_VulkanUnInitialize vulkanUnInitializeFunc_;
};


}  // namespace driver
}  // namespace vulkan