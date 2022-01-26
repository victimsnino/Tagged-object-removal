set(TARGET NetworkExecutor)

add_library(${TARGET} 
    STATIC 
        NetworkExecutor.cpp
        NetworkExecutor.h
)

target_include_directories(${TARGET} PRIVATE ${OpenCV_INCLUDE_DIRS} PUBLIC .)
target_link_libraries( ${TARGET} PRIVATE ${OpenCV_LIBS} pybind11::embed)

add_custom_command(TARGET ${TARGET} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${OPENCV_FILES_TO_COPY}
    $<TARGET_FILE_DIR:${TARGET}>)

set_target_properties(${TARGET} PROPERTIES FOLDER modules)
