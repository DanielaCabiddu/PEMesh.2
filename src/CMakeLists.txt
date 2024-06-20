cmake_minimum_required (VERSION 2.8.11)
project (PEMesh)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Charts REQUIRED)

find_package(OpenGL REQUIRED)
add_definitions(-DGL_GLEXT_PROTOTYPES)

add_definitions(-DCINOLIB_USES_QT)
add_definitions(-DCINOLIB_USES_OPENGL)
add_definitions(-DCINOLIB_USES_BOOST)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../external/cinolib/include)
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/../external/cinolib/external/eigen)

## triangle

set (TRIANGLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/triangle)
set (TRIANGLE_BUILD ${TRIANGLE_DIR}/build-${CMAKE_SYSTEM_NAME})

if (MSVC)
    set (TRIANGLE_LIB ${TRIANGLE_BUILD}/triangle.lib)
else()
    set (TRIANGLE_LIB ${TRIANGLE_BUILD}/libtriangle.a)
endif()

file (MAKE_DIRECTORY ${TRIANGLE_BUILD})

add_custom_command(
  OUTPUT ${TRIANGLE_LIB}
  COMMAND cmake .. && cmake --build .
  #DEPENDS ${SOURCE_FILES} /tmp/bin/create_foo_hh main.cpp
  WORKING_DIRECTORY ${TRIANGLE_BUILD}
)

FILE (GLOB HEADERS *.h meshes/*.h)
FILE (GLOB SOURCES *.cpp meshes/*.cpp)
FILE (GLOB GUI *.ui )

add_executable (${PROJECT_NAME} ${GUI} ${HEADERS} ${SOURCES} ${TRIANGLE_LIB} ${VTK_LIB})

target_include_directories (${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

qt5_use_modules(${PROJECT_NAME} Core Widgets OpenGL Network Charts)

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${TRIANGLE_LIB} GLU GL)
