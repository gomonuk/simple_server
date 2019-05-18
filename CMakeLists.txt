cmake_minimum_required(VERSION 3.12)
project(simple_server)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package( Threads REQUIRED)
#find_library(LIBEVENT2_CORE event_core)
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(/usr/lib/x86_64-linux-gnu)


find_path(LIBEVENT2_CORE evhttp.h
        PATHS /usr/include)


find_path(JANSSON_INCLUDE_DIR jansson.h
        /usr/include
        /usr/local/include )

find_library(JANSSON_LIBRARY NAMES jansson
        PATHS /usr/lib /usr/local/lib /usr/local/include)

set(JANSSON_LIBRARIES ${JANSSON_LIBRARY} )
set(JANSSON_INCLUDE_DIRS ${JANSSON_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set JANSSON_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Jansson  DEFAULT_MSG
        JANSSON_LIBRARY JANSSON_INCLUDE_DIR)


add_executable( simple_server main.cpp)
#target_link_libraries(kursach ${CMAKE_THREAD_LIBS_INIT} )
target_link_libraries(
        simple_server ${LIBEVENT2_CORE}
        Threads::Threads
        /usr/lib/x86_64-linux-gnu/libevent.so
        /usr/local/lib/libjansson.so
        )