cmake_minimum_required(VERSION 3.23)
project(backup_tool)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


include_directories(.)
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
add_executable(backup_tool
        backup.cpp file_info.h utils.h compress.cpp compress.h encrypt.h main.cpp mainwindow.cpp mainwindow.h restore.cpp)
target_link_libraries(backup_tool
        Qt::Core
        Qt::Gui
        Qt::Widgets
        )



