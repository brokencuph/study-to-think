# CMakeList.txt : CMake project for study-to-think, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Add source to this project's executable.
add_executable (stt "cli_main.cpp" "db_access.cpp" "grading_scheme.cpp" "student.cpp" "cli_main.h")
target_link_libraries(stt PRIVATE unofficial::sqlite3::sqlite3)

add_executable(stt_gui WIN32 "mainwindow.cpp" "mainwindow.h" "mainwindow.ui" "qt_main.cpp" "db_access.cpp" "student.cpp" "qt_defs.h" "grading_scheme.cpp" "grading_scheme.h" "manualscoredialog.cpp" "manualscoredialog.h" "manualscoredialog.ui")
target_link_libraries(stt_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets unofficial::sqlite3::sqlite3)
# TODO: Add tests and install targets if needed.
