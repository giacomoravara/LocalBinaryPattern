cmake_minimum_required(VERSION 3.15)
project(prototipo)
find_package (Threads)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES AbstractLbp.cpp AbstractLbp.h Benchmark.cpp Benchmark.h LbpSeq.cpp LbpSeq.h LbpPar.h LbpPar.cpp lodepng.cpp lodepng.h kernel.cu Types.h
        test-image-1080.png
        test-image-4k.png
        test-image-low.png
       )
add_executable(prototipo ${SOURCE_FILES})
target_link_libraries(prototipo ${CMAKE_THREAD_LIBS_INIT})