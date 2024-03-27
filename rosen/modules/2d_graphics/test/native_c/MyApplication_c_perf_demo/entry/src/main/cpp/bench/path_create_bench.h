
#ifndef PATH_CREATE_BENCH_H
#define PATH_CREATE_BENCH_H
#include "../test_base.h"
#include "bench/random_path_bench.h"

class PathCreateBench : public RandomPathBench {
  public:
    PathCreateBench() : RandomPathBench() {}
    ~PathCreateBench() = default;
  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;
};
#endif // PATH_CREATE_BENCH_H