cmake_minimum_required(VERSION 3.0.2)
project(epos_2wheels)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(EPOS_SRC
    main.cpp
    epos_single/epos_base.cpp
    two_wheels.cpp
)
set(CPP_LINK
    -lEposCmd
    -lyaml-cpp
    -lpthread
)

add_executable(epos_2wheels ${EPOS_SRC})

target_link_libraries(epos_2wheels ${CPP_LINK})

