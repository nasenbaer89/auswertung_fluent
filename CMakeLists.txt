cmake_minimum_required(VERSION 2.6)
project(auswertung_fluent)

add_executable(auswertung_fluent main.cpp)

install(TARGETS auswertung_fluent RUNTIME DESTINATION bin)
