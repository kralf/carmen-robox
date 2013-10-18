remake_find_package(Threads)
if(NOT ${CMAKE_USE_PTHREADS_INIT})
  message(FATAL_ERROR "Missing POSIX thread support!")
endif(NOT ${CMAKE_USE_PTHREADS_INIT})
remake_find_library(rt time.h)

remake_define(LASER_USE_PTHREAD ON)

remake_add_library(
  tcp862_laser_dev
  LINK tcp862_serial ${CARMEN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    ${RT_LIBRARY}
)
remake_add_headers()
