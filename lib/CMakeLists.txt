project(lib)

set(SOURCE_FILES
    wordle.h
    wordle.cpp
    trie.h
    trie.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
