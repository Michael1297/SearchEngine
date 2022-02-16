set(GUMBO_SOURCES
        gumbo-parser/src/attribute.h
        gumbo-parser/src/attribute.c
        gumbo-parser/src/char_ref.h
        gumbo-parser/src/char_ref.c
        gumbo-parser/src/error.h
        gumbo-parser/src/error.c
        gumbo-parser/src/gumbo.h
        gumbo-parser/src/insertion_mode.h
        gumbo-parser/src/parser.h
        gumbo-parser/src/parser.c
        gumbo-parser/src/string_buffer.h
        gumbo-parser/src/string_buffer.c
        gumbo-parser/src/string_piece.h
        gumbo-parser/src/string_piece.c
        gumbo-parser/src/tag_enum.h
        gumbo-parser/src/tag_gperf.h
        gumbo-parser/src/tag_sizes.h
        gumbo-parser/src/tag_strings.h
        gumbo-parser/src/tag.c
        gumbo-parser/src/token_type.h
        gumbo-parser/src/tokenizer.h
        gumbo-parser/src/tokenizer.c
        gumbo-parser/src/tokenizer_states.h
        gumbo-parser/src/utf8.h
        gumbo-parser/src/utf8.c
        gumbo-parser/src/util.h
        gumbo-parser/src/util.c
        gumbo-parser/src/vector.h
        gumbo-parser/src/vector.c)

if (MSVC)
    include_directories(gumbo-parser/visualc/include)
    set(GUMBO_SOURCES ${GUMBO_SOURCES} gumbo-parser/visualc/include/strings.h)
endif()

add_library(gumbo-parser STATIC ${GUMBO_SOURCES})
target_include_directories(gumbo-parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/gumbo-parser/src)