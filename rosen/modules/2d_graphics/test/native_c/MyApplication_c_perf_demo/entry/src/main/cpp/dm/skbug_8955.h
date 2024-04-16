#ifndef SKBUG_8955_H
#define SKBUG_8955_H
#include "../test_base.h"

class SkBug_8955 : public TestBase {
  public:
    SkBug_8955();
    ~SkBug_8955() = default;

  protected:
    void OnTestFunction(OH_Drawing_Canvas *canvas) override;
};
#endif // SKBUG_8955_H
