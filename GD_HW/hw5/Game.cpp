#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdlib>
#include <cstdio>
#include <chrono>
#include <thread>

#include "Scene.h"

using namespace glm;

namespace Game {
	Scene *scene;
	GLFWwindow *window;

	glm::vec2 press_mouse_position;
	glm::vec2 current_mouse_position;
	bool is_mouse_press;

	float angle_of_selected_bone;
	glm::vec3 axis_of_selected_bone;

	static void error_callback(int error, const char *description) {
		fprintf(stderr, "Error: %s\n", description);
	}

	static void key_callback(GLFWwindow *curWindow, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9 && action == GLFW_PRESS) {
			scene->SelectBone(key - GLFW_KEY_1);
		} else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			scene->SwitchMode();
		} else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			scene->PlayPause();
		} else if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F19 && action == GLFW_PRESS) {
			scene->SetKeyFrame(key - GLFW_KEY_F1);
		}
	}

	static void mouse_callback(GLFWwindow *curWindow, int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			press_mouse_position = glm::vec2(x, y);
			is_mouse_press = true;

			auto selectedBone = scene->GetSelectedBone();
			if (selectedBone != nullptr) {
				glm::quat quat = glm::quat_cast(selectedBone->getLocalTransformation());
				angle_of_selected_bone = glm::angle(quat);
				axis_of_selected_bone = glm::axis(quat);
			}
		} else
			is_mouse_press = false;
	}

	static void cursor_position_callback(GLFWwindow *curWindow, double xpos, double ypos) {
		current_mouse_position = glm::vec2(xpos, ypos);
	}

	glm::vec2 viewport2camera(glm::vec2 point) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		float ratio = (float) width / (float) height;

		return glm::vec2(point.x * 150 * ratio / (float) width - 75 * ratio, -point.y * 150 / (float) height + 75);
	}

	void initScene() {
		scene = new Scene();

		scene->AddBone("", "Bone 1", 25, 0);
		scene->AddBone("Bone 1", "Bone 2", 10, 0);
		scene->AddBone("Bone 2", "Bone 3", 20, 0);
		scene->AddBone("Bone 3", "Bone 4", 10, 0);
		scene->AddBone("Bone 4", "Bone 5", 20, 0);
		scene->AddBone("Bone 4", "Bone 5L", 10, glm::radians(45.0f));
//		scene->AddBone("Bone 4", "Bone 5R", 10, -glm::radians(45.0f));
		scene->AddBone("Bone 5", "Bone 6", 10, 0);
		scene->AddBone("Bone 6", "Bone 7", 10, 0);
//		scene->AddBone("Bone 7", "Bone HL", 10, glm::radians(45.0f));
//		scene->AddBone("Bone 7", "Bone HR", 10, glm::radians(-45.0f));
		scene->AddBone("Bone 7", "Bone H", 15, 0);

		scene->Init();

		const int DefSegments = 100;

		std::vector<Vertex> skin;
		int bone_count = scene->GetBoneCount();
		float x = 0;
		for (int i = 0; i < bone_count; i++) {
			int previous_bone_index = max(0, i - 1);
			int next_bone_index = min(bone_count - 1, i + 1);

			//Bone *previous_bone = scene->GetBone(previous_bone_index);
			Bone *bone = scene->GetBone(i);
			//Bone *next_bone = scene->GetBone(next_bone_index);

			// I add Variable segment count
			// it depends on bone length
			int Segments = (int) round((float) bone->getLength() / 10 * DefSegments);
			float step = (float) bone->getLength() / Segments;
			for (int j = 0; j < Segments; j++) {
				x += step;
				for (auto y : {5.f, -5.f}) {
					float part = (float) j / Segments;
					if (j < Segments / 2)
						skin.push_back(Vertex{
								x, y, previous_bone_index, i,
								0.5f - part, 0.5f + part,
								0, 0
						});
					else
						skin.push_back(Vertex{
								x, y, i, next_bone_index,
								(float) (1.0f - ((float) j - Segments / 2.0f) / Segments),
								(float) (((float) j - Segments / 2.0f) / Segments),
								0, 0
						});
				}
			}
		}

		scene->SetSkin(skin);
	}

	void init() {
		glClearColor(0, 0, 0, 1);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		initScene();
	}

	void processInput() {
		if (scene->mode == Scene::MODE_INVERSE) {
			scene->inverse_target = viewport2camera(press_mouse_position);
		} else if (scene->mode == Scene::MODE_NORMAL && is_mouse_press) {
			auto diff = (current_mouse_position - press_mouse_position);
			auto angle = glm::atan(-diff.y, diff.x);
			scene->RotateSelectedBone(angle + axis_of_selected_bone.z * angle_of_selected_bone);
		}

		glfwPollEvents();
	}

	void Update(float deltaTime) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		float ratio = float(width) / float(height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(-ratio * 75.f, ratio * 75.f, -75, 75.f);

		scene->Update(deltaTime);
	}

	void Render() {
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw Origin for rotation
		auto pos = viewport2camera(press_mouse_position);
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex2f(pos.x - 5, pos.y + 0);
		glVertex2f(pos.x + 5, pos.y + 0);
		glVertex2f(pos.x + 0, pos.y - 5);
		glVertex2f(pos.x + 0, pos.y + 5);
		glEnd();

		scene->Render();

		glfwSwapBuffers(window);
	}
}

using namespace Game;

int main() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(640, 480, "Skeletal Animation", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	glfwMakeContextCurrent(window);
	glewInit();

	init();

	glfwSwapInterval(0);
	double prevTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		double current_time = glfwGetTime();
		double delta = current_time - prevTime;

		if (delta >= 0.1) {
			processInput();
			Update(float(delta));
			Render();

			prevTime = current_time;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}