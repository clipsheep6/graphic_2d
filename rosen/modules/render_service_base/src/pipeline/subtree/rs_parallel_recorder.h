/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_RECORDER_H
#define RENDER_SERVICE_BASE_SUBTREE_RS_PARALLEL_RECORDER_H

#include <list>

namespace OHOS::Rosen {
//Forward
class RSParallelRecorder;
class RSPaintFilterCanvasBase;

using CanvasStatusOp = std::function<void(RSPaintFilterCanvasBase* canvas)>;
using RSParallelRecorderPtr = std::shared_ptr<RSParallelRecorder>;

class RSParallelRecorder final {
public:
    RSParallelRecorder() = default;

    ~RSParallelRecorder() = default;

    inline void Record(const CanvasStatusOp& op)
    {
        recorder_.emplace_back(op);
    }

    inline void Playback(RSPaintFilterCanvasBase* newCanvas)
    {
        std::for_each(recorder_.begin(), recorder_.end(), [newCanvas] (auto& oop) {
            op(newCanvas);
        });
    }

    inline int Size() const
    {
        return recorder_.size();
    }

private:
    std::list<CanvasStatusOp> recorder_;
};

template <typename T, typename R, typename ...Args>
static inline void RSParallelRecord(RSParallelRecorderPtr& recorder,
                                    R (T::*MemFunc)(Args...), Args... args)
{
    if (recorder == nullptr) {
        return ;
    }
    recorder->Record([MemFunc, args...] (T* canvas) {
        (canvas->*MemFunc)(args...);
    });
}

template <typename T, typename R, typename C, typename ...Args>
static inline void RSParallelRecord(RSParallelRecorderPtr& recorder,
                                    R (T::*MemFunc)(const C&, Args...), const C& c, Args... args)
{
    if (recorder == nullptr) {
        return ;
    }
    recorder->Record([MemFunc, c, args...] (T* canvas) {
        (canvas->*MemFunc)(c, args...);
    });
}

template <typename T, typename R, typename P1, typename P2, typename P3>
static inline void RSParallelRecord(RSParallelRecorderPtr& recorder,
                                    R (T::*MemFunc)(const P1&, P2&, P3), const P1& p1, P2& p2, P3 p3)
{
    if (recorder == nullptr) {
        return;
    }
    recorder->Record([MemFunc, p1, p2, p3] (T* canvas) mutable {
        (canvas->*MemFunc)(p1, p2, p3);
    });
}

static inline void RSParallelPlayback(RSParallelRecorderPtr& recorder, RSPaintFilterCanvasBase* newCanvas)
{
    if (recorder != nullptr && newCanvas != nullptr) {
        recorder->Playback(newCanvas);
    }
}

}
#endif