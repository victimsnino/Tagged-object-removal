add_library(RxCpp INTERFACE)
target_include_directories(RxCpp INTERFACE RxCpp/Rx/v2/src)
set_target_properties(RxCpp PROPERTIES FOLDER submodules)


add_library(RxQt INTERFACE)
target_include_directories(RxQt INTERFACE rxqt/include)
target_link_libraries(RxQt INTERFACE RxCpp)
set_target_properties(RxQt PROPERTIES FOLDER submodules)


add_subdirectory(pybind11)