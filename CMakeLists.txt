cmake_minimum_required(VERSION 3.0)
project(z1_joystick)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread")

if(${SIM})
  add_definitions(-DCOMPILE_WITH_SIMULATION)
  find_package(catkin REQUIRED COMPONENTS
    roscpp
    sensor_msgs
  )

  catkin_package(CATKIN_DEPENDS)
  include_directories(${catkin_INCLUDE_DIRS})
endif()

include_directories(
  include
)

file(GLOB_RECURSE ADD_SRC_LIST
  src/*.cpp
)

link_directories(lib)
add_library(${PROJECT_NAME}_lib ${ADD_SRC_LIST})
target_link_libraries(${PROJECT_NAME}_lib Z1_SDK_${CMAKE_SYSTEM_PROCESSOR})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${PROJECT_NAME}_lib)

