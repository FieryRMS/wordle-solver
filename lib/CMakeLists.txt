project(lib)

set(SOURCE_FILES
    wordle.h
    wordle.cpp
    trie.h
    trie.cpp
    ProgressBar.h
    ProgressBar.cpp
    Simulator.h
    Simulator.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
