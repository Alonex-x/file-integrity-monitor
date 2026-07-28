cmake_minimum_required(VERSION 3.16)
project(FileIntegrityMonitor VERSION 1.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(file-integrity-monitor
    src/main.cpp
    src/sha256.cpp
    src/integrity.cpp
    src/nexus_heartbeat.cpp
)
target_include_directories(file-integrity-monitor PRIVATE src)
target_link_libraries(file-integrity-monitor PRIVATE
    OpenSSL::Crypto
    SQLite::SQLite3
    pthread
)
