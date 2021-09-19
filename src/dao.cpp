#include <string>
#include <iostream>

#include <pqxx/pqxx>

#include "dao.h"

using namespace std::string_literals;

DAO::DAO(std::string username, std::string password, std::string host, std::string database) {
    auto connection_str = "postgresql://"s + username + ":"s + password + "@"s + host + "/"s + database;
    connection = std::move(std::make_unique<pqxx::connection>(connection_str));
    connection->prepare("insert_to_cryptodailydata", "INSERT INTO cryptodata (symbol, name, price, image_url, market_cap, volume, coingecko_timestamp) VALUES ($1, $2, $3, $4, $5, $6, $7)");
}

DAO::~DAO() {

}

void DAO::insert_records(const std::vector<CryptoCurrency> &cryptocurrencies) {
    pqxx::work txn{*connection};
    for (const CryptoCurrency &cryptocurrency : cryptocurrencies) {
        txn.exec_prepared("insert_to_cryptodailydata", 
            cryptocurrency.symbol,
            cryptocurrency.name,
            cryptocurrency.current_price,
            cryptocurrency.image_url,
            cryptocurrency.market_cap,
            cryptocurrency.volume,
            cryptocurrency.coingecko_last_updated);
    }
    txn.commit();
}
