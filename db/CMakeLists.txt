#the lib-dbmysql CMakeLists.txt
set (HEADERS 
    db_module.h
    db_mysql.h)

set (SOURCES 
    db_module.cpp
    db_mysql.cpp)


include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CAMKE_SOURCE_DIR}/xcore/include)
include_directories(.)



add_library (
    lib-dbmysql STATIC
    ${HEADERS} ${SOURCES})

target_link_libraries(lib-dbmysql
    lib-common 
    mysqlclient
    )