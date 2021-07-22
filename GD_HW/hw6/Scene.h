#pragma once

#include "Box2D.h"
#include "PhysicalEngine.h"

class Scene {
public:
	Scene() : selectedRigidBody(-1), physicalEngine(false) {}

	void init();

	void update(float dt);

	void render();

	int AddBox(float h, float w, float mass, CollisionType type = CollisionType::External);

	int AddCircle(float r, float mass, CollisionType type = CollisionType::External);

	void Select(int x, int y);

	void Select(int index);

	int GetSelectedRigidBody() const;

	void ChangeMass(float mass, bool relative = true);

	void Rotate(float angle, bool relative = true);

	void Move(float x, float y, bool isRelative = false);

	std::vector<Body2D *> GetRigidBodies();

	void ToggleResolverActivity();

	void ToggleDrawLite();

private:
	PhysicalEngine physicalEngine;
	std::vector<Body2D *> rigidBodies;
	int selectedRigidBody;
	bool drawLite = false;

};