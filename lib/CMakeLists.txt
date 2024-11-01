project(lib)

set(SOURCE_FILES
    wordle.h
    wordle.cpp
    trie.h
    trie.tpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
