cmake_minimum_required(VERSION 3.23)
project(test_work_client)

set(CMAKE_CXX_STANDARD 20)
set(path_to_conan_directory "./conan")

include(${path_to_conan_directory}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(test_work_client
        src/main.cpp
        src/include/config_structs.hpp
        src/include/config_manager.hpp
        src/source/config_manager.cpp
        src/source/logger_wrapper.cpp
        src/include/logger_wrapper.hpp
        src/include/tcp_session.hpp
        src/source/tcp_session.cpp
        )

target_link_libraries(test_work_client ${CONAN_LIBS})