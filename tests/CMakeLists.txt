# CMakeList.txt : CMake project for study-to-think, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

enable_testing()
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test1 test1.cpp "../study-to-think/db_access.cpp" "../study-to-think/student.cpp" "../study-to-think/grading_scheme.cpp")
target_link_libraries(test1 unofficial::sqlite3::sqlite3 GTest::gtest GTest::gmock)
add_executable(test_attendance "test_attendance.cpp" "../study-to-think/grading_scheme.cpp" "../study-to-think/student.cpp" "../study-to-think/db_access.cpp")
target_link_libraries(test_attendance unofficial::sqlite3::sqlite3 GTest::gtest GTest::gmock)
add_executable(test_csv "test_csv.cpp" "../study-to-think/student.cpp" "../study-to-think/db_access.cpp" "../study-to-think/grading_scheme.cpp")
target_link_libraries(test_csv unofficial::sqlite3::sqlite3 GTest::gtest GTest::gmock)
add_executable(test_grade_utils "test_grade_utils.cpp" "../study-to-think/total_grade.cpp")
target_link_libraries(test_grade_utils GTest::gtest GTest::gmock)
configure_file("test.db" "test.db" COPYONLY)
configure_file("test_crud.db" "test_crud.db" COPYONLY)

#add_test(test1 test1)
include(GoogleTest)
gtest_discover_tests(test1)
gtest_discover_tests(test_attendance)
gtest_discover_tests(test_csv)
gtest_discover_tests(test_grade_utils)