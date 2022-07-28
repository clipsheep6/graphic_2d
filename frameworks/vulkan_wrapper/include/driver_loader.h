#include "hardware/hwvulkan.h"
namespace vulkan {
namespace driver {

class DriverLoader {

public:
static bool Load();

static const DriverLoader& Get() { return loader_; }
static const hwvulkan_device_t& Device() { return *Get().dev_; }

private:
DriverLoader() : dev_(nullptr) {}
DriverLoader(const DriverLoader&) = delete;
DriverLoader& operator=(const DriverLoader&) = delete;

static DriverLoader loader_;

const hwvulkan_device_t* dev_;

};


}  // namespace driver
}  // namespace vulkan