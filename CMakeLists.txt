cmake_minimum_required(VERSION 2.6)
project(auswertung_fluent)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-strict-aliasing -std=c++11 -Werror=all -Wno-error=unused-but-set-variable -Wno-error=sign-compare")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-variable -fno-omit-frame-pointer -Wconversion")


add_executable(auswertung_fluent main.cpp)

#install(TARGETS auswertung_fluent RUNTIME DESTINATION bin)
