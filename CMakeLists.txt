cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS  "-Wall -Wextra -Werror")

project(MULTICAST)

find_package(yaml-cpp REQUIRED)

# 添加源文件的路径
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 构建可执行文件
add_executable(SimpleTikcerTest ${SOURCE_DIR}/SimpleTicker_test.cpp ${SOURCE_DIR}/Sleep.cpp)
add_executable(RandomDelayTickerTest ${SOURCE_DIR}/RandomDelayTicker_test.cpp ${SOURCE_DIR}/Sleep.cpp)
add_executable(HFMessageFormatterTest ${SOURCE_DIR}/HFMessageFormatter_test.cpp)
add_executable(LFMessageFormatterTest ${SOURCE_DIR}/LFMessageFormatter_test.cpp)
add_executable(EmulaterConfigTest ${SOURCE_DIR}/EmulaterConfig_test.cpp)
add_executable(Multicast ${SOURCE_DIR}/MulticastSender.cpp ${SOURCE_DIR}/SimpleTicker.cpp ${SOURCE_DIR}/RandomDelayTicker.cpp ${SOURCE_DIR}/HFMessageFormatter.cpp ${SOURCE_DIR}/LFMessageFormatter.cpp ${SOURCE_DIR}/Sleep.cpp ${SOURCE_DIR}/EmulaterConfig.cpp ${SOURCE_DIR}/EmulaterService.cpp)
target_link_libraries(Multicast yaml-cpp pthread)
# 链接 Google Test 框架
target_link_libraries(SimpleTikcerTest  gtest gtest_main pthread)
target_link_libraries(RandomDelayTickerTest  gtest gtest_main pthread)
target_link_libraries(HFMessageFormatterTest  gtest gtest_main pthread)
target_link_libraries(LFMessageFormatterTest  gtest gtest_main pthread)
target_link_libraries(EmulaterConfigTest  yaml-cpp gtest gtest_main pthread)