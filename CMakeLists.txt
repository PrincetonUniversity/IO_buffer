add_library(iobuf LiFo_policy.cpp  binary_to_ascii.cpp  buffer.cpp  fortranAPI_handler.cpp  fortran_api.cpp  mapper.cpp  testit.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_REENTRANT -D FORBUF_FAST")
