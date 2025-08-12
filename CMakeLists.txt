cmake_minimum_required(VERSION 3.12)
project(hackathon-sample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(opentelemetry-cpp REQUIRED CONFIG)
find_package(libpqxx REQUIRED CONFIG)
find_package(nlohmann_json REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)

# Gather all .cpp files recursively
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories for headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/util
)

target_link_libraries(hackathon-sample PRIVATE opentelemetry-cpp::opentelemetry-cpp libpqxx::pqxx nlohmann_json::nlohmann_json fmt::fmt)
