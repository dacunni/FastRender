cmake_minimum_required(VERSION 2.6)
PROJECT (fastrender)
find_package(ImageMagick COMPONENTS Magick++)
#ADD_LIBRARY (mylibrary STATIC libsource.c) 
# Build executable using specified list of source files.
file(GLOB SRCS "*.cpp")
ADD_EXECUTABLE (fr ${SRCS})
SET(CMAKE_CXX_FLAGS "-Wc++0x-compat")
include_directories(${ImageMagick_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES (fr ${ImageMagick_LIBRARIES} m)

