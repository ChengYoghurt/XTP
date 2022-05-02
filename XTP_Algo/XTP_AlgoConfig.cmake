add_library(XTP_Algo INTERFACE)
target_include_directories(
    XTP_Algo
INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_directories(
    XTP_Algo
INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/lib
)

target_link_libraries(
    XTP_Algo
INTERFACE
    #XTP::XTP
    xtpquoteapi
    xtptraderapi_xalgo
)
add_library(XTP_Algo::XTP_Algo ALIAS XTP_Algo)