#include <vector>
#include <stdexcept>

#include "coingecko_comm.h"
#include "coingecko_parser.h"
#include "types.h"
#include "http.h"

namespace {
  std::string call_coingecko(int page) {
    const std::string coingecko_uri = "https://api.coingecko.com/api/v3/coins/markets?vs_currency=usd&order=market_cap_desc&per_page=100&page=" + std::to_string(page);
    return http_request("GET", coingecko_uri);
  }
}

std::vector<CryptoCurrency> coingecko::current_prices(int page) {
  if (page <= 0) {
    throw std::runtime_error("Invalid page number for coingecko price API");
  }
  std::string result = call_coingecko(page);
  std::vector<CryptoCurrency> result_vec = coingecko::parser::parse(result);
  return result_vec;
}