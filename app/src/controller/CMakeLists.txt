set(TARGET Controller)

add_library(${TARGET} STATIC
    Controller.cpp
    Controller.h
)

target_link_libraries(${TARGET} PRIVATE ${QT_TARGET}::Widgets  ${OpenCV_LIBS} RxQtLoop ViewInterface NetworkExecutor)
target_include_directories(${TARGET} PUBLIC . PRIVATE ${OpenCV_INCLUDE_DIRS})