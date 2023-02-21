#include <unistd.h>
#include <dlfcn.h>

namespace OHOS {
int close(int fd)
{
    int (*original_close)(int);
    original_close = (int(*)(int))dlsym(RTLD_NEXT, "close");
    return original_close(fd);
}
}