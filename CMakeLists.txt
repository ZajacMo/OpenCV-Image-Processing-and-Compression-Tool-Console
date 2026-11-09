cmake_minimum_required(VERSION 3.10)
project(OpenCVImageCompressionTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenCV REQUIRED)

add_executable(compression_tool
    Compress_gray.cpp
    Bits.cpp
    Block.cpp
    RBlock.cpp
    main.cpp
)

target_include_directories(compression_tool PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(compression_tool PRIVATE ${OpenCV_LIBS})

# The codec measures its own runtime with QueryPerformanceCounter, which is
# Windows specific.
if(WIN32)
    target_link_libraries(compression_tool PRIVATE kernel32)
else()
    message(WARNING
        "This project uses the Windows QueryPerformanceCounter API and will not "
        "build on non-Windows platforms.")
endif()

install(TARGETS compression_tool RUNTIME DESTINATION bin)

enable_testing()

add_test(NAME selftest
         COMMAND compression_tool --selftest --output ${CMAKE_CURRENT_BINARY_DIR})
