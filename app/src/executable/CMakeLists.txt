
add_executable(TaggedObjectRemoval
    main.cpp
)

target_link_libraries(TaggedObjectRemoval PRIVATE ${QT_TARGET}::Widgets)