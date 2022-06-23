#include <EGL/egl.h>

extern "C" {
typedef void* (*GetNextLayerAddr)(void*, const char*);
__eglMustCastToProperFunctionPointerType DebugLayerInitialize(const void *funcTbl, GetNextLayerAddr getAddr);
__eglMustCastToProperFunctionPointerType DebugLayerGetProcAddr(const char *name, __eglMustCastToProperFunctionPointerType next);
};
