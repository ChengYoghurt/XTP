add_library(XTP INTERFACE)
target_include_directories(
    XTP
INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_directories(
    XTP
INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/bin
    ${CMAKE_CURRENT_LIST_DIR}/lib
)

target_link_libraries(
    XTP
INTERFACE
    xtpquoteapi
)
add_library(XTP::XTP ALIAS XTP)