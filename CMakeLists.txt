cmake_minimum_required(VERSION 3.9)
project(qode-edit)

set(LIB_VERSION_MAJOR 0)
set(LIB_VERSION_MINOR 1)
set(LIB_VERSION_TWEAK 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_TWEAK})

set(CMAKE_CXX_STANDARD 14)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Generate compile_commands.json in build/ for analyzers like clang-tidy.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find includes in corresponding build directories
if(NOT QT_VERSION)
  set(QT_VERSION Qt5)
endif()

find_package(${QT_VERSION} REQUIRED COMPONENTS Core Gui Widgets)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

set(RESOURCES
    resources/qode-edit.qrc
)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/resources)

add_library(qodeedit STATIC
    ${RESOURCES}
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(qodeedit PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/3rdparty/variant
)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(qodeedit
        PRIVATE
        -pedantic
        -Wall
        -Wextra
        -Woverloaded-virtual
        -Winit-self
        -Wunreachable-code
    )
endif()

target_link_libraries(qodeedit PRIVATE
    ${QT_VERSION}::Core
    ${QT_VERSION}::Widgets
    ${QT_VERSION}::Gui
)
