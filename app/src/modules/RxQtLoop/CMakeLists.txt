set(TARGET RxQtLoop)

add_library(${TARGET} 
    STATIC 
        rxqt_loop_singleton.cpp 
        rxqt_loop_singleton.h
)

target_include_directories(${TARGET} PUBLIC .)
target_link_libraries(${TARGET} PUBLIC RxCpp PRIVATE RxQt ${QT_TARGET}::Widgets)


set_target_properties(${TARGET} PROPERTIES FOLDER modules)

