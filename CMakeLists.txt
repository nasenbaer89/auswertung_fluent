cmake_minimum_required(VERSION 2.6)
project(auswertung_fluent)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-strict-aliasing -std=c++11  ")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fno-omit-frame-pointer ")


add_executable(auswertung_fluent main.cpp)

#install(TARGETS auswertung_fluent RUNTIME DESTINATION bin)
