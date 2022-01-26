set(TARGET RxQtLoop)

add_library(${TARGET} INTERFACE)
target_include_directories(${TARGET} INTERFACE .)
target_link_libraries(${TARGET} INTERFACE RxQt)


set_target_properties(${TARGET} PROPERTIES FOLDER modules)

