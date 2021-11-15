#include "animation_impl.h"

namespace OHOS {
AnimationImpl::AnimationImpl(AnimationType type)
{
    this->animationType = type;
}

AnimationImpl::~AnimationImpl()
{

}

sptr<Window> AnimationImpl::GetWindow() const
{

    return NULL;
}

GSError AnimationImpl::SetDuration(uint32_t duration)
{
    this->duration = duration;
    return GSERROR_OK;
}

GSError AnimationImpl::SetDelay(int32_t delay)
{
    this->delay = delay;
    return GSERROR_OK;
}

GSError AnimationImpl::SetRepeatCount(uint32_t count)
{
    this->count = count;
    return GSERROR_OK;
}

GSError AnimationImpl::SetTimingFunction(timingCallback func)
{
    this->timingFunc = func;
    return GSERROR_OK;
}

GSError AnimationImpl::SetRegion(int32_t x, int32_t y, int32_t w, int32_t h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    return GSERROR_OK;
}

GSError AnimationImpl::Start()
{
    return GSERROR_OK;
}

std::vector<double> AnimationImpl::GetValue() const
{
    return this->value;
}

uint32_t AnimationImpl::GetDuration() const
{
    return this->duration;
}

int32_t AnimationImpl::GetDelay() const
{
    return this->delay;
}

uint32_t AnimationImpl::GetRepeatCount() const
{
    return this->count;
}

GSError AnimationImpl::OnStart(callback func)
{
    this->onStartFunc = func;
    return GSERROR_OK;
}

GSError AnimationImpl::OnEnd(callback func)
{
    this->onEndFunc = func;
    return GSERROR_OK;
}

GSError AnimationImpl::OnRepeat(callback func)
{
    this->onRepeatFunc = func;
    return GSERROR_OK;
}

GSError AnimationImpl::OnCancel(callback func)
{
    this->onCancelFunc = func;
    return GSERROR_OK;
}

GSError AnimationImpl::SetValue(std::vector<double> &v)
{
    this->value = v;
    return GSERROR_OK;
}
} // namespace OHOS
