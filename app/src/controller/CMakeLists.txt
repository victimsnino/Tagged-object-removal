set(TARGET Controller)

add_library(${TARGET} STATIC
    Controller.cpp
    Controller.h
)

target_link_libraries(${TARGET} PRIVATE RxQtLoop ViewInterface)
target_include_directories(${TARGET} PUBLIC .)