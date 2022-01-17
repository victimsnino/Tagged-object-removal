add_library(rxcpp INTERFACE)
target_include_directories(rxcpp INTERFACE RxCpp/Rx/v2/src)

add_library(rxqt INTERFACE)
target_include_directories(rxqt INTERFACE rxqt/include)
target_link_libraries(rxqt INTERFACE rxcpp)

add_subdirectory(pybind11)