#include <vector>
#include <stdexcept>
#include <string>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Array.h>
#include <Poco/Dynamic/Var.h>

#include "coingecko_parser.h"
#include "types.h"

using namespace Poco::JSON;
using namespace std::string_literals;

namespace {
  CryptoCurrency deserialize_entry(Object::Ptr entry) {
    using std::string;
    string id = entry->getValue<string>("id"s);
    string symbol = entry->getValue<string>("symbol"s);
    string name = entry->getValue<string>("name"s);
    string image_url = entry->getValue<string>("image");
    double current_price = entry->getValue<double>("current_price");
    double market_cap = entry->getValue<double>("market_cap");
    double volume = entry->getValue<double>("total_volume");
    string coingecko_last_upd = entry->getValue<string>("last_updated");
    return {
      id,
      symbol,
      name,
      image_url,
      current_price,
      market_cap,
      volume,
      coingecko_last_upd
    };
  }
}

std::vector<CryptoCurrency> coingecko::parser::parse(std::string input) {
  Parser parser;
  Poco::Dynamic::Var var = parser.parse(input);
  std::vector<CryptoCurrency> result;
  if (var.isArray()) {
    Array::Ptr ptr = var.extract<Array::Ptr>();
    for (size_t i = 0; i < ptr->size(); i++) {
      CryptoCurrency currency = deserialize_entry(ptr->getObject(i));
      result.push_back(currency);
    }
  } else {
    throw std::runtime_error("Invalid coingecko JSON");
  }
  return result;
}