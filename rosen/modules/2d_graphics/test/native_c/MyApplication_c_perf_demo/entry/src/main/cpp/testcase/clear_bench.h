#ifndef CLEAR_BENCH_TEST_H
#define CLEAR_BENCH_TEST_H
#include "test_base.h"
enum ClearType {
    kFull_ClearType,
    kPartial_ClearType,
    kComplex_ClearType
};
class ClearBench : public TestBase {
    ClearType fType;

  public:
    ClearBench(ClearType type) : fType(type) {}
    ~ClearBench() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override;

  private:
    ClearType type;
};
#endif // CLEAR_BENCH_TEST_H