#include "animation_set_impl.h"

namespace OHOS{
AnimationSetImpl::AnimationSetImpl()
{
    std::vector<std::shared_ptr<Animation>> animations;

}

AnimationSetImpl::~AnimationSetImpl()
{

}

std::shared_ptr<AnimationSet> AnimationSetImpl::Get()
{
    std::shared_ptr<AnimationSet> animationSet = std::make_shared<AnimationSetImpl>();
    return animationSet;
}

GSError AnimationSetImpl::Insert(std::shared_ptr<Animation> &animation, int32_t time)
{
    return GSERROR_OK;
}

GSError AnimationSetImpl::With(std::shared_ptr<Animation> &animation)
{
    return GSERROR_OK;
}

GSError AnimationSetImpl::Then(std::shared_ptr<Animation> &animation, int32_t time)
{
    return GSERROR_OK;
}

GSError AnimationSetImpl::Start()
{
    return GSERROR_OK;
}

std::vector<std::shared_ptr<Animation>> AnimationSetImpl::GetAnimations() const
{
    std::vector<std::shared_ptr<Animation>> test;
    return test;
}

} // namespace OHOS
