cmake_minimum_required(VERSION 3.10)
project(MicaClient)

# Curl library
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Adding sources
set(SOURCES
    main.cpp
    internal/message.cpp
    internal/messagememory.cpp
    internal/display.cpp
    internal/aes.cpp
    internal/requests.cpp
    internal/tools.cpp
    internal/arguments.cpp
    internal/colors.cpp
    internal/importer.cpp
)

# Binary
add_executable(MicaClient ${SOURCES})

# Link Curl library
target_link_libraries(MicaClient ${CURL_LIBRARY})