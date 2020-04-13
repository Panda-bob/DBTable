#the lib-dbmysql CMakeLists.txt
set (HEADERS 
    #../frame/app_config.h
    db_module.h
    db_mysql.h
    )

set (SOURCES 
    #../frame/app_config.cpp
    db_module.cpp
    db_mysql.cpp)


include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/xcore/include)
include_directories(${PROJECT_SOURCE_DIR}/frame/)
include_directories(/usr/local/mysql/include)
include_directories(.)



add_library (
    lib-dbmysql STATIC
    ${HEADERS} ${SOURCES})

target_link_libraries(lib-dbmysql
    lib-common 
    mysqlclient
    )