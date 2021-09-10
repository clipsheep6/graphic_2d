#include <iostream>

#include "driver.h"

namespace OHOS {
Driver::Driver() :
    _parser(_scanner, *this), _scanner(*this)
{
}

int Driver::parse()
{
  _scanner.switch_streams(std::cin, std::cout);
  return _parser.parse();
}
} // namespace OHOS
