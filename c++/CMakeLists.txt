cmake_minimum_required(VERSION 3.1)

project(bluezero_camera_opencv)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR 1)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_AUTOMOC ON)

if(WIN32)
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DBOOST_USE_WINDOWS_H)
    add_definitions(-DNOMINMAX)
endif()

find_package(Boost 1.54 REQUIRED COMPONENTS thread system regex timer filesystem serialization)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)
#find_package(bluezero REQUIRED)
set(B0_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../bluezero" CACHE STRING "BlueZero directory")
set(B0_BUILD_DIR "${B0_DIR}/build" CACHE STRING "BlueZero build directory")
set(B0_LIBRARY "${B0_BUILD_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}b0${CMAKE_SHARED_LIBRARY_SUFFIX}" CACHE STRING "BlueZero library")

include_directories(${B0_DIR}/include)
include_directories(${B0_BUILD_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIR})

add_executable(camera-node camera-node.cpp)
target_link_libraries(camera-node ${B0_LIBRARY} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES})

add_executable(gui-node gui-node.cpp)
target_link_libraries(gui-node ${B0_LIBRARY} ${OpenCV_LIBRARIES} ${Boost_LIBRARIES} Qt5::Core Qt5::Gui Qt5::Widgets)

