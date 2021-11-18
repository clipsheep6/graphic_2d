
#include "wmclient_native_test_32.h"

#include <cstdio>

#include <display_type.h>
#include <window_manager.h>

#include "inative_test.h"
#include "native_test_class.h"
#include "util.h"

using namespace OHOS;

namespace{
class WMClientNativeTest32 : public INativeTest {
public:
    std::string GetDescription() const override
    {
        constexpr const char *desc = "egl rotate test";
        return desc;
    }

    std::string GetDomain() const override
    {
        constexpr const char *domain = "wmclient";
        return domain;
    }

    int32_t GetID() const override
    {
        constexpr int32_t id = 32;
        return id;
    }

    uint32_t GetLastTime() const override
    {
        constexpr uint32_t lastTime = LAST_TIME_FOREVER;
        return lastTime;
    }

    void Run(int32_t argc, const char **argv) override
    {
        auto initRet = WindowManager::GetInstance()->Init();
        if (initRet) {
            printf("init failed with %s\n", WMErrorStr(initRet).c_str());
            ExitTest();
            return;
        }

        window = NativeTestFactory::CreateWindow(WINDOW_TYPE_NORMAL);
        if (window == nullptr) {
            ExitTest();
            return;
        }

        window->SwitchTop();
        auto producer = window->GetProducer();
        sptr<EglRenderSurface> pEglSurface = EglRenderSurface::CreateEglRenderSurfaceAsProducer(producer);
        windowSync = NativeTestSync::CreateSyncEglRotate(NativeTestDraw::FlushDrawEglRotate,
                pEglSurface, window->GetWidth(), window->GetHeight()); 
    }

private:
    sptr<Window> window = nullptr;
    sptr<NativeTestSync> windowSync = nullptr;
} g_autoload;
}
