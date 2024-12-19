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
    wordleRegression.h
    wordleRegression.cpp
    wordleLoop.h
    wordleLoop.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
