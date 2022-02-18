cmake_minimum_required(VERSION 3.20)

add_executable(${PROJECT_NAME}
        main.cpp
        SearchEngine.cpp
        ${INCLUDE_DIR}/SearchEngine.h
        )

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})