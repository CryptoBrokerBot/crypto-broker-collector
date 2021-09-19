#pragma once

#include <vector>
#include <memory>

#include <pqxx/pqxx>

#include "types.h"

class DAO {
public:
  DAO(std::string username, std::string password, std::string host, std::string database);
  virtual ~DAO();
  void insert_records(const std::vector<CryptoCurrency> &cryptocurrencies);

private:
  std::unique_ptr<pqxx::connection> connection;
};
