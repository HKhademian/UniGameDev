cmake_minimum_required(VERSION 3.16)
project(SimpleVision)

set(CMAKE_CXX_STANDARD 20 C CPP)

set(GL -lGL -lGLU -lglut)
set(GLFW -lglfw)
set(GLEW -lGLEW)
set(FREE_GLUT -lfreeglut)

add_executable(
        simple-vision
        Game.cpp Scene.hpp Object.hpp
)
target_link_libraries(simple-vision ${GL} ${GLEW} ${GLUT} ${GLFW})
