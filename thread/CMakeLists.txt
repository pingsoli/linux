cmake_minimum_required(VERSION 3.0)

#set(CMAKE_BUILD_TYPE Debug)

add_executable(pthread pthread.c)
target_link_libraries(pthread -pthread)

add_executable(mutex mutex.c)
target_link_libraries(mutex -pthread)

add_executable(race_condition race_condition.c)
target_link_libraries(race_condition -pthread)

add_executable(semaphore semaphore.c)
target_link_libraries(semaphore -pthread rt)
