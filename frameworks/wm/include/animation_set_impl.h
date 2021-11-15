#ifndef FRAMEWORKS_WM_INCLUDE_ANIMATION_SET_H
#define FRAMEWORKS_WM_INCLUDE_ANIMATION_SET_H

#include <animation_set.h>

namespace OHOS {
class AnimationSetImpl : public AnimationSet {
public:
    AnimationSetImpl();
    virtual ~AnimationSetImpl();

    static std::shared_ptr<AnimationSet> Get();

    virtual GSError Insert(std::shared_ptr<Animation> &animation, int32_t time) override;
    virtual GSError With(std::shared_ptr<Animation> &animation) override;
    virtual GSError Then(std::shared_ptr<Animation> &animation, int32_t time) override;
    virtual GSError Start() override;

    virtual std::vector<std::shared_ptr<Animation>> GetAnimations() const override;

private:

};

} // namespace OHOS

#endif // FRAMEWORKS_WM_INCLUDE_ANIMATION_SET_H
