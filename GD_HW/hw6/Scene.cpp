#define GLEW_STATIC

#include <GL/glew.h>

#include "Scene.h"
#include "CollisionDetector.h"

void Scene::init() {
	selectedRigidBody = -1;
	ToggleResolverActivity();
	//physicalEngine.SetResolverAcivity(true);

//	AddBox(100, 150, 1e8, CollisionType::Internal);
	AddCircle(100, 1e8, CollisionType::Internal);
}

void Scene::update(float dt) {
	physicalEngine.SetRigidBodies(&this->rigidBodies);
	physicalEngine.Update(dt);
}

void Scene::render() {
	glPushAttrib(GL_LINE_BIT);

	// Draw Origin
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(-20, 0);
	glVertex2f(20, 0);
	glVertex2f(0, -20);
	glVertex2f(0, 20);
	glEnd();

	// Draw Box
	for (int i = 0; i < this->rigidBodies.size(); i++) {
		auto body = this->rigidBodies[i];
		auto mass = body->GetMass();
		auto vertices = body->GetVertices(drawLite);
		auto isInternal = body->GetCollisionType() == CollisionType::Internal;

		if (isInternal || this->selectedRigidBody == i)
			glColor3f(0, 0, 1);
		else
			glColor3f(0, 1, 0);

		glLineWidth(glm::min(2.f, mass));

		glBegin(GL_LINE_LOOP);
		for (auto v : vertices)
			glVertex2f(v.x, v.y);
		glEnd();
	}

	// Draw Collision info
	auto collisionInfo = physicalEngine.CurrentCollisionInfo();
	for (auto info : collisionInfo) {
		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex2f(info.penetrationPoint.x, info.penetrationPoint.y);
		glVertex2f(info.contactPoint.x, info.contactPoint.y);
		glEnd();
	}

	glPopAttrib();
}

int Scene::AddBox(float h, float w, float mass, CollisionType type) {
	auto *body = new Box2D(h, w, mass, type);
	this->rigidBodies.push_back(body);

	physicalEngine.SetChangedRigidBodies(true);

	return this->rigidBodies.size() - 1;
}

int Scene::AddCircle(float r, float mass, CollisionType type) {
	auto *body = new Circle2D(r, mass, type);
	this->rigidBodies.push_back(body);

	physicalEngine.SetChangedRigidBodies(true);

	return this->rigidBodies.size() - 1;
}

void Scene::Select(int x, int y) {
	auto p = glm::vec2(x, y);
	for (int i = 0; i < this->rigidBodies.size(); i++) {
		auto body = this->rigidBodies[i];
		if (body->GetCollisionType() == CollisionType::Internal) continue;

		if (CollisionDetector::ShapeContainsPoint(body, p)) {
			selectedRigidBody = i;
			return;
		}
	}
}

void Scene::Select(int index) {
	selectedRigidBody = glm::min((int) this->rigidBodies.size() - 1, glm::max(0, index));
}

int Scene::GetSelectedRigidBody() const {
	return selectedRigidBody;
}

void Scene::ChangeMass(float mass, bool relative) {
	if (selectedRigidBody >= 0) {
		this->rigidBodies[selectedRigidBody]->ChangeMass(mass, relative);
		physicalEngine.SetChangedRigidBodies(true);
	}
}

void Scene::Rotate(float angle, bool relative) {
	if (selectedRigidBody >= 0) {
		this->rigidBodies[selectedRigidBody]->Rotate(angle, relative);
		physicalEngine.SetChangedRigidBodies(true);
	}
}

void Scene::Move(float x, float y, bool isRelative) {
	if (selectedRigidBody >= 0) {
		this->rigidBodies[selectedRigidBody]->MoveTo(x, y, isRelative);
		physicalEngine.SetChangedRigidBodies(true);
	}
}

void Scene::ToggleResolverActivity() {
	static bool ResolverActivity = false;
	ResolverActivity = !ResolverActivity;
	physicalEngine.SetResolverAcivity(ResolverActivity);
}

void Scene::ToggleDrawLite() {
	drawLite = !drawLite;
}

std::vector<Body2D *> Scene::GetRigidBodies() {
	return this->rigidBodies;
}
