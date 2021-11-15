#ifndef INTERFACES_INNERKITS_WMCLIENT_ANIMATION_SET_H
#define INTERFACES_INNERKITS_WMCLIENT_ANIMATION_SET_H

#include <memory>
#include <vector>

#include "animation.h"
#include "wm_common.h"

namespace OHOS {
class AnimationSet {
public:
    static std::shared_ptr<AnimationSet> Get();

    virtual GSError Insert(std::shared_ptr<Animation> &animation, int32_t time) = 0;
    virtual GSError With(std::shared_ptr<Animation> &animation) = 0;
    virtual GSError Then(std::shared_ptr<Animation> &animation, int32_t time) = 0;
    virtual GSError Start() = 0;

    virtual std::vector<std::shared_ptr<Animation>> GetAnimations() const = 0;

};
} // namespace OHOS

#endif // INTERFACES_INNERKITS_WMCLIENT_ANIMATION_SET_H
