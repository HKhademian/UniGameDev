#include <vector>
#include <GL/glew.h>
#include <glm/vec2.hpp>

#include "Object.hpp"

class Scene {
public:
	Scene() :
			selected(-1),
			vision_range(25),
			vision_angle(60),
			vision_pos(0, 0) {}

	void init();

	void update(float dt);

	void render();


	[[nodiscard]]
	int GetSelected() const { return selected; }

	[[nodiscard]]
	std::vector<Object *> GetObjects() const { return objects; }

	void Select(int x, int y);

	void Select(int index);

	void Rotate(float angle, bool relative = true);

	void Move(float x, float y, bool isRelative = false);


	void SetVisionPos(float x, float y) { vision_pos = glm::vec2(x, y); }


	int AddObject(int segments, float radius, float angle, float x, float y);


private:
	float vision_range;
	float vision_angle;
	glm::vec2 vision_pos;

	std::vector<Object *> objects;
	int selected;
};

void Scene::init() {
	selected = -1;
	vision_pos = glm::vec2(0, 0);
	AddObject(5, 5, 0, 1, 1);
}

void Scene::update(float dt) {
	// TODO update vision_pos

	for (auto object : objects) {
		object->visible = false;
		for (auto v : object->vertices) {
			if (glm::length(v - vision_pos) < vision_range) {
				object->visible = true;
				break;
			}
		}
	}

}

void drawPlus(float x, float y, float size) {
	glBegin(GL_LINES);
	glVertex2f(x - size, y);
	glVertex2f(x + size, y);
	glVertex2f(x, y - size);
	glVertex2f(x, y + size);
	glEnd();
}

void drawPoly(std::vector<glm::vec2> vertices) {
	glBegin(GL_LINE_LOOP);
	for (auto v : vertices)
		glVertex2f(v.x, v.y);
	glEnd();
}

void Scene::render() {
	glPushAttrib(GL_LINE_BIT);

	// Draw Origin
	glColor3f(1, 0, 0);
	drawPlus(0, 0, 20);

//	// Draw Vision
//	glColor3f(0, 1, 0);
//	drawPlus(vision_pos.x, vision_pos.y, 5);

	// draw field of view
//	{
//		auto rot = glm::rotate(glm::mat4(1), glm::radians(vision_angle), glm::vec3(0, 0, 1));
//		auto pos0 = vision_pos;
//		auto pos1 = pos0 + glm::vec2(rot * glm::vec4(vision_range, 0, 0, 1));
//		auto pos2 = pos0 + glm::vec2(rot * glm::vec4(0, vision_range, 0, 1));
//
	glColor3f(0, 1, 1);
	glLineWidth(2.0f);
//		glBegin(GL_LINES);
//		glVertex2f(pos0.x, pos0.y);
//		glVertex2f(pos1.x, pos1.y);
//		glVertex2f(pos0.x, pos0.y);
//		glVertex2f(pos2.x, pos2.y);
//		glEnd();
//	}
	drawPoly(CircleVertices(vision_range, vision_angle, vision_pos));


	// Draw Box
	for (auto object : this->objects) {
		auto isVisible = object->visible;
		auto vertices = object->vertices;

		if (isVisible)
			glColor3f(0, 0, 1);
		else
			glColor3f(1, 0, 0);
		glLineWidth(2.0f);

		drawPoly(vertices);
	}

	glPopAttrib();
}

int Scene::AddObject(int segments, float radius, float angle, float x, float y) {
	auto obj = new Object{segments, radius, angle, glm::vec2(x, y)};
	obj->GenerateVertices();
	this->objects.push_back(obj);
	return int(this->objects.size() - 1);
}

void Scene::Select(int x, int y) {
	auto p = glm::vec2(x, y);
	selected = -1;
	for (int i = 0; i < this->objects.size(); i++) {
		auto object = this->objects[i];
		auto dist = glm::length(p - object->position);
		if (dist < object->radius) {
			selected = i;
			return;
		}
	}
}

void Scene::Select(int index) {
	selected = int(this->objects.size() + index) % this->objects.size();
}

void Scene::Rotate(float angle, bool isRelative) {
	if (selected >= 0) {
		if (isRelative)
			this->objects[selected]->angle += angle;
		else
			this->objects[selected]->angle = angle;
		this->objects[selected]->GenerateVertices();
	}
}

void Scene::Move(float x, float y, bool isRelative) {
	if (selected >= 0) {
		if (isRelative)
			this->objects[selected]->position += glm::vec2(x, y);
		else
			this->objects[selected]->position = glm::vec2(x, y);
		this->objects[selected]->GenerateVertices();
	}
}
