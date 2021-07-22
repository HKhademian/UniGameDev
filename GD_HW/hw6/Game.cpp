#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "Scene.h"

#include "Game.h"
#include <random>
#include <thread>

namespace PhysicalSimulationGame {
	Scene *scene;
	GLFWwindow *mainWindow;

	glm::vec2 press_mouse_position;
	glm::vec2 current_mouse_position;
	bool is_mouse_press = false;
	bool create_shape = false;
	bool create_circle = false;
	float changeRotation;

	glm::vec2 viewport2camera(glm::vec2 point);

	static void error_callback(int error, const char *description) {
		fprintf(stderr, "Error: %s\n", description);
	}

	static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (mods == GLFW_MOD_CONTROL) {
			switch (key) {
				case GLFW_KEY_UP:
					scene->Move(0, .1, true);
					return;
				case GLFW_KEY_DOWN:
					scene->Move(0, -.1, true);
					return;
				case GLFW_KEY_LEFT:
					scene->Move(-.1, 0, true);
					return;
				case GLFW_KEY_RIGHT:
					scene->Move(.1, 0, true);
					return;
			}
		}

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
			scene->Select(key - GLFW_KEY_1);
		} else if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			scene->ChangeMass(1);
		} else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			scene->ChangeMass(-1);
		} else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			scene->Select(scene->GetSelectedRigidBody() - 1);
		} else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			scene->Select(scene->GetSelectedRigidBody() + 1);
		} else if (key == GLFW_KEY_R) {
			changeRotation = 1;
		} else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
			create_shape = true;
		} else if (key == GLFW_KEY_V && action == GLFW_PRESS) {
			create_circle = true;
		} else if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
			scene->ToggleResolverActivity();
		} else if ((key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT) && action == GLFW_PRESS) {
			scene->ToggleDrawLite();
		}
	}

	static void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
		if (action == GLFW_RELEASE) {
			press_mouse_position = current_mouse_position;
		}

		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			press_mouse_position = glm::vec2(x, y);
			is_mouse_press = true;

			static auto before = std::chrono::system_clock::now();
			auto now = std::chrono::system_clock::now();
			double diff_ms = std::chrono::duration<double, std::milli>(now - before).count();
			before = now;
			if (diff_ms > 10 && diff_ms < 250) {
				create_shape = true;
			} else {
				auto pos = viewport2camera(glm::vec2(x, y));
				scene->Select(pos.x, pos.y);
			}
		} else
			is_mouse_press = false;
	}

	static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
		current_mouse_position = glm::vec2(xpos, ypos);
	}

	static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
		changeRotation = float(yoffset);
	}

	static void resize_callback(GLFWwindow *window, int width, int height) {
		double ratio = float(width) / (float) height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-ratio * 75.f, ratio * 75.f, -75, 75.f);
	}

	glm::vec2 viewport2camera(glm::vec2 point) {
		float ratio;
		int width, height;

		glfwGetFramebufferSize(mainWindow, &width, &height);
		ratio = float(width) / float(height);

		return glm::vec2(point.x * 150 * ratio / float(width) - 75 * ratio, -point.y * 150 / float(height) + 75);
	}

	void init() {
		glClearColor(0, 0, 0, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		scene = new Scene();
		scene->init();
	}

	void Update(float dt) {
		scene->update(dt);
	}

	void Render() {
		glClear(GL_COLOR_BUFFER_BIT);

		auto pos = viewport2camera(is_mouse_press ? current_mouse_position : press_mouse_position);
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(pos.x - 5, pos.y + 0);
		glVertex2f(pos.x + 5, pos.y + 0);
		glVertex2f(pos.x + 0, pos.y - 5);
		glVertex2f(pos.x + 0, pos.y + 5);
		glEnd();

		scene->render();

		glfwSwapBuffers(mainWindow);
	}

	void processInput() {
		if (create_shape) {
			create_shape = false;
			float h = 10 + (rand() / float(RAND_MAX)) * 20;
			float w = 10 + (rand() / float(RAND_MAX)) * 20;
			float m = 1 + (rand() / float(RAND_MAX)) * 3;
			scene->Select(scene->AddBox(h, w, m));
			auto pos = viewport2camera(press_mouse_position);
			scene->Move(
					pos.x + (rand() / float(RAND_MAX)),
					pos.y + (rand() / float(RAND_MAX))
			);
		} else if (create_circle) {
			create_circle = false;
			float r = 3 + (rand() / float(RAND_MAX)) * 10;
			float m = 1 + (rand() / float(RAND_MAX)) * 5;
			scene->Select(scene->AddCircle(r, m));
			auto pos = viewport2camera(press_mouse_position);
			scene->Move(
					pos.x + (rand() / float(RAND_MAX)),
					pos.y + (rand() / float(RAND_MAX))
			);
		} else if (is_mouse_press && current_mouse_position != press_mouse_position) {
			auto pos = viewport2camera(current_mouse_position);
			scene->Move(pos.x, pos.y);
		} else if (changeRotation != 0) {
			scene->Rotate(changeRotation / 30.0f, true);
			changeRotation = 0;
		}

		glfwPollEvents();
	}

}

using namespace PhysicalSimulationGame;

int main() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 8);

	auto window = mainWindow = glfwCreateWindow(640, 480, "Rigid Body Simulation", NULL, NULL);
	if (!window) {
		glfwTerminate();

		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowSizeCallback(window, resize_callback);

	glfwMakeContextCurrent(window);

	glewInit();

	init();

	glfwSwapInterval(0);

//double previous_time = glfwGetTime();
//while (!glfwWindowShouldClose(window))
//{
//    double current_time = glfwGetTime();
//    double delta = current_time - previous_time;

//    if (delta >= 0.1)
//    {
//        processInput();
//        Update(0.1);
//        Render();

//        prevTime = current_time;
//    }
//    else
//    {
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//    }
//}

	static const float MS_PER_RENDER = 0.1;
	static const float MS_PER_UPDATE = 0.001;

	double previous_time = glfwGetTime();
	double previous_render_time = previous_time;
	double lag = 0.0;

	while (!glfwWindowShouldClose(window)) {
		double current_time = glfwGetTime();
		double elapsed = current_time - previous_time;
		double delta = current_time - previous_render_time;

		previous_time = current_time;
		lag += elapsed;

		processInput();

		while (lag >= MS_PER_UPDATE) {
			Update(MS_PER_UPDATE);
			lag -= MS_PER_UPDATE;
		}

		if (delta >= MS_PER_RENDER) {
			Render(/*lag / MS_PER_UPDATE*/);

			previous_render_time = current_time;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10)
			);
		}
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}