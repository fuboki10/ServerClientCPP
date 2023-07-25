cmake_minimum_required(VERSION 3.10)

project(Test)

find_package( Threads REQUIRED )

message(${CMAKE_THREAD_LIBS_INIT})
message(${CMAKE_DL_LIBS})

add_library(ThreadSafeQueue SHARED ThreadSafeQueue.cpp ThreadSafeQueue.h)
target_include_directories(ThreadSafeQueue PUBLIC ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR})


add_executable(server server.cpp)
add_executable(client client.cpp)


target_link_libraries(server PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} ThreadSafeQueue)
target_link_libraries(client PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} ThreadSafeQueue)