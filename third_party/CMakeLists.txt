file(GLOB GUMBO_SOURCES
        "gumbo-parser/src/*.h"
        "gumbo-parser/src/*.c")

set(mINI
        mINI/src/mini/ini.h)

set(base64
        base64/include/base64.hpp)

set(ThreadPool
        ThreadPool/ThreadPool.h)

add_library(third_party_library STATIC ${GUMBO_SOURCES} mINI base64 ThreadPool)
target_include_directories(third_party_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gumbo-parser/src)
target_include_directories(third_party_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})