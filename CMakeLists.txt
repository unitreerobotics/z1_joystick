cmake_minimum_required(VERSION 3.0)
project(z1_joystick)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread")

option(SIM "use F710 joystick" OFF)

if(${SIM})
  add_definitions(-DCOMPILE_WITH_SIMULATION)
  find_package(catkin REQUIRED COMPONENTS
    roscpp
    sensor_msgs
  )

  catkin_package(CATKIN_DEPENDS)
  include_directories(${catkin_INCLUDE_DIRS})
endif()


find_package(Eigen3 REQUIRED)
include_directories(
  include
  ${Eigen3_INCLUDE_DIRS}
)

file(GLOB_RECURSE ADD_SRC_LIST
  src/*.cpp
)

link_directories(lib)
add_library(${PROJECT_NAME}_lib ${ADD_SRC_LIST})
target_link_libraries(${PROJECT_NAME}_lib Z1_SDK_Linux64)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${PROJECT_NAME}_lib)

