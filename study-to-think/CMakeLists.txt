# CMakeList.txt : CMake project for study-to-think, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (stt "cli_main.cpp" "db_access.cpp")
target_link_libraries(stt PRIVATE unofficial::sqlite3::sqlite3)
# TODO: Add tests and install targets if needed.
