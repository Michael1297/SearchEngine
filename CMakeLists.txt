cmake_minimum_required(VERSION 3.20)

project(SearchEngine)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_CPR_TESTS OFF)
set(CMAKE_USE_OPENSSL OFF)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Boost ============================================================
set(Boost_INCLUDE_DIR "C:\\boost_1_77_0")
set(Boost_LIBRARY_DIR "C:\\boost_1_77_0\\stage\\lib")
find_package(Boost COMPONENTS REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
#===================================================================

add_subdirectory(third_party/cpr)
add_subdirectory(third_party/json)
add_subdirectory(third_party)                           #gumbo-parser + ini parser
add_subdirectory(third_party/OleanderStemmingLibrary)   #стимминг слов
add_subdirectory(third_party/nanodbc)                   #управление SQL бд
add_subdirectory(third_party/fmt)                       #Форматирование строк

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE third_party_library)
target_link_libraries(${PROJECT_NAME} PRIVATE oleanderstemminglibrary)
target_link_libraries(${PROJECT_NAME} PRIVATE nanodbc "${ODBC_LINK_FLAGS}")
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

#копирование файла
configure_file(config.ini ${CMAKE_CURRENT_BINARY_DIR}/config.ini COPYONLY)