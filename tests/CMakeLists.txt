# CMakeList.txt : CMake project for study-to-think, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

enable_testing()
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test1 test1.cpp "../study-to-think/db_access.cpp" "../study-to-think/student.cpp")
target_link_libraries(test1 unofficial::sqlite3::sqlite3 GTest::gtest GTest::gmock)

#add_test(test1 test1)
include(GoogleTest)
gtest_discover_tests(test1)