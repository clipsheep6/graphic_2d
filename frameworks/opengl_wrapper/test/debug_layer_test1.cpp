#include "debug_layer_test.h"

#include <string>

#include "../src/wrapper_log.h"

namespace {
constexpr const char *MY_LOG_TAG = "DebugLayerTest1";
}
static GetNextLayerAddr gGetNextLayerAddr = nullptr;
static void *gFuncTbl = nullptr;

static void *GetNextLayerProc(const char *name)
{
    if (gGetNextLayerAddr && gFuncTbl) {
        return gGetNextLayerAddr(gFuncTbl, name);
    }
    WLOGD("-%{public}s- GetNextLayerProc init error.", MY_LOG_TAG);
    return nullptr;
}

static EGLDisplay EglGetDisplay(EGLNativeDisplayType type)
{
    WLOGD("-%{public}s-", MY_LOG_TAG);
    PFNEGLGETDISPLAYPROC next = (PFNEGLGETDISPLAYPROC)GetNextLayerProc("eglGetDisplay");
    if (next) {
        return next(type);
    }
    return EGL_NO_DISPLAY;
}

__eglMustCastToProperFunctionPointerType DebugLayerInitialize(const void *funcTbl, GetNextLayerAddr getAddr)
{
    WLOGD("-%{public}s-", MY_LOG_TAG);
    gFuncTbl = const_cast<void *>(funcTbl);
    gGetNextLayerAddr = getAddr;
    return nullptr;
}

__eglMustCastToProperFunctionPointerType DebugLayerGetProcAddr(const char *name,
    __eglMustCastToProperFunctionPointerType next)
{
    WLOGD("-%{public}s- ===name[%{public}s]===", MY_LOG_TAG, name);
    std::string func(name);
    if (func == "eglGetDisplay") {
        WLOGD("-%{public}s-", MY_LOG_TAG);
        return (__eglMustCastToProperFunctionPointerType)EglGetDisplay;
    }

    return next;
}
