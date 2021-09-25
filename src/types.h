#pragma once

#include <string>

#ifndef types_h
#define types_h

struct CryptoCurrency {
  std::string id;
  std::string symbol;
  std::string name;
  std::string image_url;
  double current_price;
  double market_cap;
  double volume;
  std::string coingecko_last_updated;
};

#endif