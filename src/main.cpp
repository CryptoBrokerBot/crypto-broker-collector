#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

#include "coingecko_comm.h"
#include "types.h"
#include "dao.h"

void print_record(std::string symbol, std::string name, std::string curr_price) {
    char buff[128];
    sprintf(buff, "%-10s%40s%20s", symbol.c_str(), name.c_str(), curr_price.c_str());
    std::cout << buff << std::endl;
}

void print_ruler(int width) {
    while (width--) {
        std::cout << "=";
    }
    std::cout << std::endl;
}

void load_prices(std::string db_user, std::string db_pass, std::string db_host, std::string db_name) {
    int coin_ct = 0;
    print_record("SYMBOL", "NAME", "PRICE");
    print_ruler(10+40+20);
    DAO dao{db_user, db_pass, db_host, db_name};
    int page = 1;
    while (coin_ct < 500) {
        std::vector<CryptoCurrency> coins = coingecko::current_prices(page);
        dao.insert_records(coins);
        for (const CryptoCurrency &coin : coins) {
            print_record(coin.symbol, coin.name, std::to_string(coin.current_price));
        }
        page++;
        coin_ct += coins.size();

        if (coin_ct < 500) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
    }
}

std::string getenv_string(std::string env) {
    const char *env_value = std::getenv(env.c_str()); 
    if (env_value == nullptr) {
        throw std::runtime_error("Unknown environment variable");
    }
    return std::string(env_value);
}

void hourly_task() {
    try {
        std::string db_user = getenv_string("CB_DBUSER");
        std::string db_pass = getenv_string("CB_DBPASS");
        std::string db_host = getenv_string("CB_DBHOST");
        std::string db_name = getenv_string("CB_DBNAME");

        load_prices(db_user, db_pass, db_host, db_name);
    } catch (const std::exception &ex) {
        std::cout << "ERROR: " << ex.what() << std::endl;
    }
}

int main() {
    for (;;) {
        hourly_task();
        std::this_thread::sleep_for(std::chrono::hours(1));
    }
}
