add_executable (tests-beast
    ${BOOST_BEAST_FILES}
    ${EXTRAS_FILES}
    Jamfile
    core.cpp
    http.cpp
    ssl.cpp
    version.cpp
    websocket.cpp
    zlib.cpp
)

target_link_libraries(tests-beast
    lib-asio
    lib-asio-ssl
    lib-beast
    lib-test)

set_property(TARGET tests-beast PROPERTY FOLDER "tests")