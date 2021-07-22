cmake_minimum_required(VERSION 3.16)
project(GD_HW)

set(CMAKE_CXX_STANDARD 20 C CPP)


# sudo apt install libglm-dev
set(GL -lGL -lGLU -lglut)

set(SFWL -lsfml-graphics -lsfml-window -lsfml-system)

set(GLFW -lglfw)

# sudo apt install -y libglew-dev
set(GLEW -lGLEW)

# sudo apt install libsoil-dev
set(SOIL -lSOIL)

#
set(FREE_GLUT -lfreeglut)


add_executable(
        test-cubes
        test/cubes.cpp
)
target_link_libraries(test-cubes ${GL} ${GLEW} ${GLUT} ${GLFW})

add_executable(
        hw06-physics
        hw6/Game.cpp hw6/Game.h
        hw6/Scene.cpp hw6/Scene.h
        hw6/Box2D.cpp hw6/Box2D.h
        hw6/CollisionDetector.cpp hw6/CollisionDetector.h
        hw6/CollisionResolver.cpp hw6/CollisionResolver.h
        hw6/PhysicalEngine.cpp hw6/PhysicalEngine.h
        hw6/simulation.cpp
)
target_link_libraries(hw06-physics ${GL} ${GLEW} ${GLUT} ${GLFW})

add_executable(
        hw05-kinematic
        hw5/Game.cpp
        hw5/Bone.cpp hw5/Bone.h
        hw5/Scene.cpp hw5/Scene.h
        hw5/Skeleton.cpp hw5/Skeleton.h
)
target_link_libraries(hw05-kinematic ${GL} ${GLEW} ${GLUT} ${GLFW})

#add_executable(
#        hw01_boxes
#        hw1/CommandPattern-9830339.cpp
#)
#
#add_executable(
#        hw02-marpele-classic
#        hw2v3/util.hpp
#        hw2v3/Dice.hpp
#        hw2v3/EventSystem.hpp
#        hw2v3/Terminal.hpp
#        hw2v3/Colors.hpp
#        hw2v3/MarPele.hpp
#        hw2v3/MarPele-classic.cpp
#)
#
#
#add_executable(
#        hw02-marpele-extended
#        hw2v3/util.hpp
#        hw2v3/Dice.hpp
#        hw2v3/EventSystem.hpp
#        hw2v3/Terminal.hpp
#        hw2v3/Colors.hpp
#        hw2v3/MarPele.hpp
#        hw2v3/MarPele-extended.cpp
#)
#
#add_executable(
#        hw02-mench-classic
#        hw2v3/util.hpp
#        hw2v3/Dice.hpp
#        hw2v3/EventSystem.hpp
#        hw2v3/Terminal.hpp
#        hw2v3/Colors.hpp
#        hw2v3/Mench.hpp
#        hw2v3/Mench-classic.cpp
#)
#
#add_executable(
#        hw04-map
#        hw4/Renderer.cpp
#        hw4/map.cpp
#)
#target_link_libraries(hw04-map ${GL} ${GLEW} ${GLUT} ${SOIL})

add_executable(
        test
        test.cpp
)
