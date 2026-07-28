cmake_minimum_required(VERSION 3.16)
project(FileIntegrityMonitor VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)

add_executable(file-integrity-monitor src/main.cpp src/sha256.cpp src/integrity.cpp)
target_include_directories(file-integrity-monitor PRIVATE src)
target_link_libraries(file-integrity-monitor PRIVATE OpenSSL::Crypto)
