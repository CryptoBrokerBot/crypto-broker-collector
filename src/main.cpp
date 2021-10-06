#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <thread>

#include "coingecko_comm.h"
#include "types.h"
#include "dao.h"
#include "syncQueue.hpp"

//Global SyncQueue for storing retrieved cryptocoins from coingecko
SyncQueue<std::vector<CryptoCurrency>> coinsQueue{};

constexpr int COIN_COUNT = 500;
constexpr int COIN_GECKO_SPAM_DELAY = 10000;

void print_record(std::string rank,std::string symbol, std::string name, std::string curr_price) {
    char buff[128];
    sprintf(buff, "%-5s%10s%40s%20s", rank.c_str(), symbol.c_str(), name.c_str(), curr_price.c_str());
    std::cout << buff << std::endl;
}

void print_ruler(int width) {
    while (width--) {
        std::cout << "=";
    }
    std::cout << std::endl;
}

void retrieve_coins() {

    int coin_ct = 0;
    print_record("RANK","SYMBOL", "NAME", "PRICE");
    print_ruler(5+10+40+20);
    int page = 1;

    // Keep getting coins till we get to COIN_COUNT of them
    while (coin_ct < COIN_COUNT) {
        std::vector<CryptoCurrency> coins = coingecko::current_prices(page);
        coinsQueue.add(coins);

        page++;
        coin_ct += coins.size();

        // sleep for 10 seconds just so that we dont spam coingecko
        if (coin_ct < COIN_COUNT) {
            std::this_thread::sleep_for(std::chrono::milliseconds(COIN_GECKO_SPAM_DELAY));
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

void periodically_retrieve_coins() {
    for (;;) {
        try {
            retrieve_coins();
        } catch (const std::exception &ex) {
            std::cout << "ERROR: " << ex.what() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::hours(1));
    }

}

void continous_insert_coin_batch_into_database() {
    std::string db_user = getenv_string("CB_DBUSER");
    std::string db_pass = getenv_string("CB_DBPASS");
    std::string db_host = getenv_string("CB_DBHOST");
    std::string db_name = getenv_string("CB_DBNAME");

    DAO dao{db_user, db_pass, db_host, db_name};

    while (true)
    {
        try {
            auto coins = coinsQueue.poll();
            dao.insert_records(coins);
            for (const CryptoCurrency &coin : coins) {
                print_record(std::to_string(coin.market_cap_rank), coin.symbol, coin.name, std::to_string(coin.current_price));  
            }
        } catch (const std::exception &ex) {
            std::cout << "ERROR: " << ex.what() << std::endl;
        }
        
    }
    
}

int main() {
    std::thread coin_insertion_thread {continous_insert_coin_batch_into_database};
    std::thread coin_retrieval_thread {periodically_retrieve_coins};
    
    coin_insertion_thread.join();
    coin_retrieval_thread.join();
}