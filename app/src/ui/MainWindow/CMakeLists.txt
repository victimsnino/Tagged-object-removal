set(TARGET MainWindow)

qt_add_library(${TARGET}
    SOURCES
        MainWindow.cpp
        MainWindow.h
    UI
        MainWindow.ui
)

target_link_libraries(${TARGET} PUBLIC ViewInterface PRIVATE ${QT_TARGET}::Widgets RxQt)
target_include_directories(${TARGET} PUBLIC .)

set_target_properties(${TARGET} PROPERTIES FOLDER UI)
