#the lib-common CMakeLists.txt
set (HEADERS 
    common.h)

set (SOURCES 
    common.cpp)


#include_directories(${CMAKE_SOURCE_DIR}/common)
#include_directories(${CAMKE_SOURCE_DIR}/xcore/include)
include_directories(.)



add_library (
    lib-common STATIC
    ${HEADERS} ${SOURCES})

#target_link_libraries(lib-cache
    #mysqlclient)