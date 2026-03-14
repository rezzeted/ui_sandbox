add_library(drui fonts.cpp)
target_include_directories(drui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(drui PUBLIC cxx_std_20)
target_link_libraries(drui PUBLIC imgui)
