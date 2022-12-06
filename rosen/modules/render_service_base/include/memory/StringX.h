//
//
//

#ifndef WGR_GRAPHIC_STRINGX_H
#define WGR_GRAPHIC_STRINGX_H
#include <string>
#include <stdarg.h>
namespace OHOS::Rosen {
class StringX : public std::string {
public:
    StringX() : std::string() {}

    StringX(const char* c) : std::string(c) {}

    void appendFormatV(const char* fmt, va_list args)
    {
        int n;
        va_list tmp_args;

        va_copy(tmp_args, args);
        n = vsnprintf(nullptr, 0, fmt, tmp_args);
        va_end(tmp_args);

        if (n>0) {
            char buf[n+1];
            vsnprintf(buf, n+1, fmt, args);
            append(buf);
        }
    }

    void appendFormat(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);

        appendFormatV(fmt, args);

        va_end(args);
    }

    void LOG(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        appendFormatV(fmt, args);
        va_end(args);

        append("\n");
    }

    static StringX& GetInstance()
    {
        static StringX instance;
        return instance;
    }
};
} // namespace
#endif // WGR_GRAPHIC_STRINGX_H