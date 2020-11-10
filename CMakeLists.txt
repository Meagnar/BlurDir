project(BlurDir)
cmake_minimum_required(VERSION 3.10)

add_executable(embedfile src/embedder.c)

add_custom_command(
  OUTPUT faceCascadeData.c
  COMMAND embedfile faceCascadeData ${CMAKE_SOURCE_DIR}/misc/haarcascade_frontalface_alt.xml
  DEPENDS embedfile)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_DEBUG_LIBS         OFF)
set(Boost_USE_RELEASE_LIBS       ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BUILD_SHARED_LIBS            OFF)
set(CMAKE_INSTALL_RPATH    "$ORIGIN")

find_package(Boost 1.60 REQUIRED COMPONENTS filesystem)
find_package(OpenCV REQUIRED)
find_package(GTest REQUIRED)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

add_executable(LibTest src/libtest.cpp faceCascadeData.c)
target_link_libraries(LibTest ${Boost_LIBRARIES})
target_link_libraries(LibTest GTest::GTest GTest::Main)
target_link_libraries(LibTest ${OpenCV_LIBS})
add_custom_command(TARGET LibTest POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/misc/Test_files $<TARGET_FILE_DIR:LibTest>/Test_files)

add_library(FindFaces MODULE src/library.cpp faceCascadeData.c)
target_link_libraries(FindFaces ${Boost_LIBRARIES})
target_link_libraries(FindFaces ${OpenCV_LIBS})

add_executable(BlurDir src/main.cpp)
target_link_libraries(BlurDir ${Boost_LIBRARIES})
target_link_libraries(BlurDir ${OpenCV_LIBS})

install(TARGETS BlurDir FindFaces DESTINATION bin)
install(TARGETS LibTest DESTINATION bin/Test)
install(DIRECTORY misc/Test_files DESTINATION bin/Test)
