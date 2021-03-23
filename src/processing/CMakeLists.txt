project(processing)
add_executable(processing processing.cpp)
target_link_libraries(processing ${OpenCV_LIBS})
