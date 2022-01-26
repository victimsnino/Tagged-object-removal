
add_executable(TaggedObjectRemoval
    main.cpp
)

target_link_libraries(TaggedObjectRemoval 
    PRIVATE 
        RxQtLoop
        RxQt
        
        ${QT_TARGET}::Widgets

        MainWindow
        Controller
)