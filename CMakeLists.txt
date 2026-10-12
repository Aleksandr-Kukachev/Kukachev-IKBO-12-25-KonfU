cmake_minimum_required(VERSION 3.16)
project(UnixShellEmulator LANGUAGES CXX)

set(Qt6_DIR "C:/vcpkg/installed/x64-windows/share/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(BUILD_GUI "Build Qt GUI" ON)
if(MSVC)
    add_compile_options(/utf-8)
endif()
add_library(shell_core src/Parser.cpp src/Shell.cpp src/Config.cpp src/ScriptRunner.cpp src/VFS.cpp)
target_include_directories(shell_core PUBLIC src)
if(BUILD_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
    add_executable(shell-emulator src/main.cpp src/MainWindow.cpp)
    target_link_libraries(shell-emulator PRIVATE shell_core Qt6::Widgets)
endif()
include(CTest)
if(BUILD_TESTING)
    add_executable(shell-demo tests/demo.cpp)
    target_link_libraries(shell-demo PRIVATE shell_core)
    add_executable(shell-tests tests/tests.cpp)
    target_link_libraries(shell-tests PRIVATE shell_core)
    add_test(NAME shell-tests COMMAND shell-tests "${CMAKE_CURRENT_SOURCE_DIR}")
endif()

if(WIN32)
    add_custom_command(TARGET shell-emulator POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory
            "$<TARGET_FILE_DIR:shell-emulator>/platforms"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>"
            "$<TARGET_FILE_DIR:shell-emulator>/platforms/"
    )
endif()