cmake_minimum_required (VERSION 3.10)

project(CryptoBrokerCollector)

find_library(POCO_CRYPTO PocoCrypto)
find_library(POCO_NET_SSL PocoNetSSL)
find_library(POCO_UTIL PocoUtil)
find_library(POCO_DATA PocoData)
find_library(POCO_JSON PocoJSON)
find_library(POCO_FOUNDATION PocoFoundation)
find_library(POCO_NET PocoNet)
find_library(PQXX NAMES pqxx pq)

add_executable(
  CryptoBrokerCollector 
  src/main.cpp 
  src/coingecko_comm.cpp 
  src/coingecko_parser.cpp 
  src/dao.cpp
)

# target_include_directories(CryptoBrokerCollector PUBLIC /usr/include/)

# https://stackoverflow.com/a/57685932
# To resolve "undefined reference to `pthread_create'"
find_package (Threads REQUIRED)
target_link_libraries(CryptoBrokerCollector
    
)

target_link_libraries(CryptoBrokerCollector 
  PUBLIC ${POCO_FOUNDATION}
  PUBLIC ${POCO_NET}
  PUBLIC ${POCO_JSON}
  PUBLIC ${POCO_DATA}
  PUBLIC ${POCO_UTIL}
  PUBLIC ${POCO_CRYPTO}
  PUBLIC ${POCO_NET_SSL}
  PUBLIC ${PQXX}
  PUBLIC ${CMAKE_THREAD_LIBS_INIT}
)

