cmake_minimum_required(VERSION 3.20)

add_executable(${PROJECT_NAME}
        main.cpp
        SearchEngine.cpp
        GumboAPI.cpp
        HttpTool.cpp
        Stemming.cpp
        ${INCLUDE_DIR}/SearchEngine.h
        ${INCLUDE_DIR}/GumboAPI.h
        ${INCLUDE_DIR}/Exception.h
        ${INCLUDE_DIR}/HttpTool.h
        ${INCLUDE_DIR}/Config.h
        ${INCLUDE_DIR}/Stemming.h
        )

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})