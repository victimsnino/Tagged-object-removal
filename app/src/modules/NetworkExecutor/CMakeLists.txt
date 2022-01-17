set(TARGET NetworkExecutor)

add_library(${TARGET})

target_include_directories(${TARGET} PRIVATE ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( ${TARGET} PRIVATE ${OpenCV_LIBS} )

add_custom_command(TARGET ${TARGET} 
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${OPENCV_FILES_TO_COPY}
    $<TARGET_FILE_DIR:${TARGET}>)