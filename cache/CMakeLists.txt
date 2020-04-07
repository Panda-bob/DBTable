#the lib-cache CMakeLists.txt
set (HEADERS 
    cache_column.h
    cache_table.h
    define.h)

set (SOURCES 
    cache_column.cpp
    cache_table.cpp)


include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CAMKE_SOURCE_DIR}/xcore/include)
include_directories(.)



add_library (
    lib-cache STATIC
    ${HEADERS} ${SOURCES})

target_link_libraries(lib-cache
    #mysqlclient
    lib-common 
    )