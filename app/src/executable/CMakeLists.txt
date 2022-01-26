
add_executable(TaggedObjectRemoval
    main.cpp
)

target_link_libraries(TaggedObjectRemoval 
    PRIVATE 
        RxQtLoop
        ${QT_TARGET}::Widgets
)