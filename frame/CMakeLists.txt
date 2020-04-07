#add_definitions(-DTEST) 

# Enable extra compiler warnings and mark warnings as errors: -Wall -Wextra -Werror
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
	option(JRTPLIB_WARNINGSASERRORS "Enable -Wall -Wextra -Werror" ON)
	if (JRTPLIB_WARNINGSASERRORS)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
	endif ()
endif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)

set (HEADERS 
    db_helper.h)

set (SOURCES 
    db_helper.cpp
    main.cpp)


include_directories(${PROJECT_SOURCE_DIR}/xcore/include)
include_directories(${PROJECT_SOURCE_DIR}/db)
include_directories(${PROJECT_SOURCE_DIR}/cache)
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(.)

add_executable (dbtable
    ${SOURCES}
    ${HEADERS})

target_link_libraries(dbtable
    lib-common    
    #mysqlclient
    lib-dbmysql
    lib-cache
    )

#set_property(TARGET dbtable PROPERTY FOLDER "dbtables")