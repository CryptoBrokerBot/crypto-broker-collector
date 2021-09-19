#pragma once

#include <vector>
#include "types.h"

namespace coingecko {
  namespace parser {
    std::vector<CryptoCurrency> parse(std::string);
  }
}