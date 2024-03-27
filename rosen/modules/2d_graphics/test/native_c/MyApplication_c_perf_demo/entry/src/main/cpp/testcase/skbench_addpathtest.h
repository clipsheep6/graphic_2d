#ifndef SKBENCH_ADDPATHTEST_TEST_H
#define SKBENCH_ADDPATHTEST_TEST_H
#include "test_base.h"
#include "random_path_bench.h"
#include <linux/bpf.h>
#include <linux/omapfb.h>
#include <string.h>
#include <string>
#include <time.h>
#include <array>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

enum AddType {
    kAdd_AddType,
    kAddTrans_AddType,
    kAddMatrix_AddType,
};

class SkBench_AddPathTest : public RandomPathBench {
public:
    SkBench_AddPathTest(AddType type) : fType(type) {}
    SkBench_AddPathTest() : RandomPathBench() {}
    ~SkBench_AddPathTest() = default;

// 初始化变量
#define SK_Scalar1 1.0f
    int width = 10;
    int i = 0;
    int horizontal_distance = 2;
    int vertical_distance = 5;
    int count = 100;

    OH_Drawing_Path **fPaths0;
    OH_Drawing_Path **fPaths1;

protected:
    enum {
        RAND_SIZE = 300,

    };
    uint32_t colors[RAND_SIZE];
    void CreateArray(int i, int kPathCnt);
    void cleanup();
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
    AddType fType;
};

#endif // SKBENCH_ADDPATHTEST_TEST_H
