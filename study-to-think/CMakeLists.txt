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
# add_executable (stt "cli_main.cpp" "db_access.cpp" "grading_scheme.cpp" "student.cpp" "cli_main.h")
# target_link_libraries(stt PRIVATE unofficial::sqlite3::sqlite3)

find_package(Qt${QT_VERSION_MAJOR}Charts)

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")

add_executable(stt_gui WIN32 "mainwindow.cpp" "mainwindow.h" "mainwindow.ui" "qt_main.cpp" "db_access.cpp" "student.cpp" "student.h" "qt_defs.h" "grading_scheme.cpp" "grading_scheme.h" "manualscoredialog.cpp" "manualscoredialog.h" "manualscoredialog.ui" "attendancescoredialog.cpp" "attendancescoredialog.h" "attendancescoredialog.ui" "stat_utils.cpp" "stat_utils.h" "csv_io.h" "csv_io.cpp" "total_grade.h" "total_grade.cpp" "aboutdialog.h" "aboutdialog.cpp"
	${APP_ICON_RESOURCE_WINDOWS} "icon.ico")
target_link_libraries(stt_gui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts unofficial::sqlite3::sqlite3)
target_include_directories(stt_gui PRIVATE "${PROJECT_BINARY_DIR}/study-to-think")

configure_file(stt_config.h.in stt_config.h)
configure_file(imgs/logo.png imgs/logo.png COPYONLY)
configure_file(imgs/icon.png imgs/icon.png COPYONLY)
# configure_file(imgs/icon.ico imgs/icon.ico COPYONLY)

# TODO: Add tests and install targets if needed.
