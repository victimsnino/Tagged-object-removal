set(TARGET ViewInterface)

add_library(${TARGET} INTERFACE ViewInterface.h)
target_include_directories(${TARGET} INTERFACE .)

set_target_properties(${TARGET} PROPERTIES FOLDER UI)
