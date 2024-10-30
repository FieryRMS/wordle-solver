project(lib)

set(SOURCE_FILES
    wordle.h
    wordle.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
