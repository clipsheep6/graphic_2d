#ifndef INTERFACES_INNERKITS_WMCLIENT_ANIMATION_H
#define INTERFACES_INNERKITS_WMCLIENT_ANIMATION_H

#include <functional>
#include <memory>
#include <vector>

#include "window.h"
#include "wm_common.h"

namespace OHOS {
using timingCallback = std::function<double(double)>;
using callback = std::function<void(void)>;
class Animation {
public:
    virtual sptr<Window> GetWindow() const = 0;    

    template<typename... Args>
    GSError SetValue(Args... args);
    virtual GSError SetDuration(uint32_t duration) = 0;
    virtual GSError SetDelay(int32_t delay) = 0; 
    virtual GSError SetRepeatCount(uint32_t count) = 0;
    virtual GSError SetTimingFunction(timingCallback func) = 0;
    virtual GSError SetRegion(int32_t x, int32_t y, int32_t w, int32_t h) = 0;
    virtual GSError Start() = 0;

    virtual std::vector<double> GetValue() const = 0;
    virtual uint32_t GetDuration() const = 0;
    virtual int32_t GetDelay() const = 0;
    virtual uint32_t GetRepeatCount() const = 0;

    virtual GSError OnStart(callback func) = 0;
    virtual GSError OnEnd(callback func) = 0;
    virtual GSError OnRepeat(callback func) = 0;
    virtual GSError OnCancel(callback func) = 0;

protected:
    virtual GSError SetValue(std::vector<double> &v) = 0;

};
} // namespace OHOS
#endif // INTERFACES_INNERKITS_WMCLIENT_ANIMATION_H
