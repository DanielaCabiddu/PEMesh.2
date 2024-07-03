cmake_minimum_required(VERSION 3.5)

project (PEMesh)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

find_package (OpenGL)

## qt

#find_package(Qt5 REQUIRED COMPONENTS Core Widgets OpenGL Network Charts)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets Gui Charts)

if (MSVC)
    qt_standard_project_setup()
endif()

## boost

find_package(Boost)

if (NOT BOOST_FOUND AND MSVC)
    if(NOT DEFINED ${CMAKE_TOOLCHAIN_FILE})
        #if(NOT DEFINED ENV{VCPKG_ROOT})
            #if(WIN32)
                set(VCPKG_ROOT $ENV{HOMEDRIVE}$ENV{HOMEPATH}/vcpkg_cpptruths)
            #else()
            #    set(VCPKG_ROOT $ENV{HOME}/.vcpkg_cpptruths)
            #endif()
        #else()
        #    set(VCPKG_ROOT $ENV{VCPKG_ROOT})
        #endif()

        if(NOT EXISTS ${VCPKG_ROOT})
            message("Cloning vcpkg in ${VCPKG_ROOT}")
            execute_process(COMMAND git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT})
            # If a reproducible build is desired (and potentially old libraries are # ok), uncomment the
            # following line and pin the vcpkg repository to a specific githash.
            # execute_process(COMMAND git checkout 745a0aea597771a580d0b0f4886ea1e3a94dbca6 WORKING_DIRECTORY ${VCPKG_ROOT})
        else()
            # The following command has no effect if the vcpkg repository is in a detached head state.
            message("Auto-updating vcpkg in ${VCPKG_ROOT}")
            execute_process(COMMAND git pull WORKING_DIRECTORY ${VCPKG_ROOT})
        endif()

        if(NOT EXISTS ${VCPKG_ROOT}/README.md)
            message(FATAL_ERROR "***** FATAL ERROR: Could not clone vcpkg *****")
        endif()

        if(WIN32)
            set(BOOST_INCLUDEDIR ${VCPKG_ROOT}/installed/x86-windows/include)
            set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg.exe)
            set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.bat)
        else()
            set(VCPKG_EXEC ${VCPKG_ROOT}/vcpkg)
            set(VCPKG_BOOTSTRAP ${VCPKG_ROOT}/bootstrap-vcpkg.sh)
        endif()

        if(NOT EXISTS ${VCPKG_EXEC})
            message("Bootstrapping vcpkg in ${VCPKG_ROOT}")
            execute_process(COMMAND ${VCPKG_BOOTSTRAP} WORKING_DIRECTORY ${VCPKG_ROOT})
        endif()

        if(NOT EXISTS ${VCPKG_EXEC})
            message(FATAL_ERROR "***** FATAL ERROR: Could not bootstrap vcpkg *****")
        endif()

        set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")

        #message(STATUS "***** Checking project third party dependencies in ${VCPKG_ROOT} *****")
        set(VCPKG_PLATFORM_TOOLSET v142)

        execute_process(
            COMMAND ${VCPKG_EXEC} install boost --triplet x64-windows-static --recurse
            WORKING_DIRECTORY ${VCPKG_ROOT})

        message ("Setting Boost MACROS")
        set (Boost_INCLUDE_DIRS "${VCPKG_ROOT}/installed/x64-windows/include" )
        message (${Boost_INCLUDE_DIRS})

    endif()
endif()

include_directories(${Boost_INCLUDE_DIRS})

## cinolib

add_definitions(-DCINOLIB_USES_BOOST)
add_definitions(-DCINOLIB_USES_TRIANGLE)
add_definitions(-DCINOLIB_USES_OPENGL)
add_definitions(-DCINOLIB_USES_QT)
add_definitions(-DCINOLIB_USES_OPENGL_GLFW_IMGUI)
include_directories( ${CMAKE_SOURCE_DIR}/../external)
include_directories( ${CMAKE_SOURCE_DIR}/../external/cinolib/include)
include_directories( ${CMAKE_SOURCE_DIR}/../external/cinolib/external/eigen)
include_directories( ${CMAKE_SOURCE_DIR}/../external/cinolib/external/imgui/glfw/include)

## triangle

set (TRIANGLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/triangle)
set (TRIANGLE_BUILD ${TRIANGLE_DIR}/build-${CMAKE_SYSTEM_NAME})

if (MSVC)
    set (TRIANGLE_LIB ${TRIANGLE_BUILD}/Debug/triangle.lib)
else()
    set (TRIANGLE_LIB ${TRIANGLE_BUILD}/libtriangle.a)
endif()

file (MAKE_DIRECTORY ${TRIANGLE_BUILD})

add_custom_command(
    OUTPUT ${TRIANGLE_LIB}
    COMMAND cmake .. && cmake --build .
    WORKING_DIRECTORY ${TRIANGLE_BUILD}
)

include_directories(${TRIANGLE_DIR})

## metis

if (LINUX)
    add_definitions(-DCINOLIB_USES_METIS)
    set(METIS_DIR ${PROJECT_SOURCE_DIR}/METIS/include)
    set(METIS_LIB /home/tommaso/local/lib/libmetis.so)
    include_directories (BEFORE ${METIS_DIR})

    set(GKLIB_DIR /home/tommaso/local/include)
    set(GKLIB_LIB /home/tommaso/local/lib/libGKlib.a)
    include_directories (BEFORE ${GKLIB_DIR})
endif()

## includes

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

FILE (GLOB HEADERS *.h meshes/*.h)
FILE (GLOB SOURCES *.cpp meshes/*.cpp)
FILE (GLOB GUI *.ui *.qrc)

qt_add_executable (${PROJECT_NAME} MANUAL_FINALIZATION ${HEADERS} ${SOURCES} ${GUI} ${TRIANGLE_LIB})

# qt5_use_modules(${PROJECT_NAME} Core Widgets OpenGL Network Charts)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt6::Core Qt6::OpenGL Qt6::Widgets Qt6::OpenGLWidgets Qt6::Gui Qt6::Charts OpenGL::GL OpenGL::GLU ${TRIANGLE_LIB} ${METIS_LIB} ${GKLIB_LIB})
qt_finalize_executable(${PROJECT_NAME})
