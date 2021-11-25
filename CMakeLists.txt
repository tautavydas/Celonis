cmake_minimum_required(VERSION 2.8)

project(Celonis CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(Cache_interactive interactive/Cache_interactive.cpp)
add_executable(Disk_interactive interactive/Disk_interactive.cpp)
add_executable(KeyValueStore_FIFO_interactive interactive/KeyValueStore_FIFO_interactive.cpp)
add_executable(KeyValueStore_LRU_interactive interactive/KeyValueStore_LRU_interactive.cpp)
add_executable(KeyValueStore_LFU_interactive interactive/KeyValueStore_LFU_interactive.cpp)

find_package(Threads REQUIRED)
target_link_libraries(Disk_interactive ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(Cache_interactive ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(KeyValueStore_FIFO_interactive ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(KeyValueStore_LRU_interactive ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(KeyValueStore_LFU_interactive ${CMAKE_THREAD_LIBS_INIT})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
add_executable(unit_tests test/unit_tests.cpp)
target_link_libraries(unit_tests ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

enable_testing()
add_test(NAME UnitTests COMMAND unit_tests)
