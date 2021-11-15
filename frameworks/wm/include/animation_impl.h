#ifndef FRAMEWORKS_WM_INCLUDE_ANIMATION_H
#define FRAMEWORKS_WM_INCLUDE_ANIMATION_H

#include <animation.h>

namespace OHOS {
class AnimationImpl : public Animation {
public:
    AnimationImpl(AnimationType type);
    virtual ~AnimationImpl();

    virtual sptr<Window> GetWindow() const override;
    
    template<typename... Args>
    GSError SetValue(Args... args)
    {
        if (sizeof...(Args) == 0)
            return GSERROR_INVALID_ARGUMENTS;
    
        double a[] = {static_cast<double>(args)...};
        std::vector<double> v;
        for (int i = 0; i < sizeof(a) / sizeof(*a); i++)
            v.push_back(a[i]);
        SetValue(v);
        return GSERROR_OK;
    }
    virtual GSError SetDuration(uint32_t duration) override;
    virtual GSError SetDelay(int32_t delay) override;
    virtual GSError SetRepeatCount(uint32_t count) override;
    virtual GSError SetTimingFunction(timingCallback func) override;
    virtual GSError SetRegion(int32_t x, int32_t y, int32_t w, int32_t h) override;
    virtual GSError Start() override;

    virtual std::vector<double> GetValue() const override;
    virtual uint32_t GetDuration() const override;
    virtual int32_t GetDelay() const override;
    virtual uint32_t GetRepeatCount() const override;

    virtual GSError OnStart(callback func) override;
    virtual GSError OnEnd(callback func) override;
    virtual GSError OnRepeat(callback func) override;
    virtual GSError OnCancel(callback func) override;

private:
    AnimationType animationType;
    std::vector<double> value;
    uint32_t duration;
    int32_t delay;
    uint32_t count;
    std::function<double(double)> timingFunc;
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
    std::function<void(void)> onStartFunc;
    std::function<void(void)> onEndFunc;
    std::function<void(void)> onRepeatFunc;
    std::function<void(void)> onCancelFunc;

protected:
    virtual GSError SetValue(std::vector<double> &v) override;

};
} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_ANIMATION_H
