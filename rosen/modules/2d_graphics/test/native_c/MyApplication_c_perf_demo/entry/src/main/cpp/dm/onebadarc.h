#ifndef ONEBADARC_H
#define ONEBADARC_H
#include "../test_base.h"

class OneBadArc : public TestBase {
  public:
    OneBadArc();
    ~OneBadArc() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // ONEBADARC_H
