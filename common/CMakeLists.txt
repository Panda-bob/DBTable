#the lib-common CMakeLists.txt
set (HEADERS 
    common.h
    interface_module.h
    app_config.h)

set (SOURCES 
    common.cpp
    app_config.cpp)


include_directories(${PROJECT_SOURCE_DIR}/common)
#include_directories(${CAMKE_SOURCE_DIR}/xcore/include)
include_directories(.)



add_library (
    lib-common STATIC
    ${HEADERS} ${SOURCES})

#target_link_libraries(lib-cache
    #mysqlclient)