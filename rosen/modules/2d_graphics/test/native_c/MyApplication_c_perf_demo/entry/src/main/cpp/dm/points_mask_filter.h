#ifndef POINTS_MASK_FILTER_H
#define POINTS_MASK_FILTER_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include "common/log_common.h"
#include "test_common.h"

class PointsMaskFilter : public TestBase {
public:
    PointsMaskFilter();
    ~PointsMaskFilter() = default;

protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // POINTS_MASK_FILTER_H