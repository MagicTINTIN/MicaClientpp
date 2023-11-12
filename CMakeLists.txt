cmake_minimum_required(VERSION 3.10)
project(MicaClient)
set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_BUILD_TYPE Debug)

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
    internal/themes.cpp
)

# Binary
add_executable(MicaClient ${SOURCES})
set_property(TARGET MicaClient PROPERTY CXX_STANDARD 17)
target_compile_features(MicaClient PRIVATE cxx_std_17)
# Link Curl library
target_link_libraries(MicaClient ${CURL_LIBRARY})