
#the top level CMakeLists.txt

cmake_minimum_required(VERSION 2.8)
project(dbtable)

add_subdirectory(db)
add_subdirectory(cache)
add_subdirectory(common)
#add_subdirectory(test)
add_subdirectory(frame)

#the c++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# debug and release config
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} --O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")