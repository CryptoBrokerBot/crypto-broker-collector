#pragma once

#include <vector>

#include "types.h"
#include "http.h"

namespace coingecko {
  std::vector<CryptoCurrency> current_prices(int page);
};
