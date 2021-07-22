#include "PhysicalEngine.h"

void PhysicalEngine::AddRigidBody(Body2D *body) {
	this->RigidBodies->push_back(body);
}

void PhysicalEngine::SetRigidBodies(std::vector<Body2D *> *rigidBodies) {
	this->RigidBodies = rigidBodies;
}

void PhysicalEngine::Update(float dt) {
	if (!isChangedRigidBodies && this->currentCollisionInfo.size() == 0) return;
	isChangedRigidBodies = false;

	// Dynamic
	this->currentCollisionInfo = detector.Detect(this->RigidBodies);
	if (this->resolveCollision)
		resolver.Resolve(this->currentCollisionInfo);
	// Impulse
}

std::vector<CollisionInfo> PhysicalEngine::CurrentCollisionInfo() {
	return this->currentCollisionInfo;
}

void PhysicalEngine::SetResolverAcivity(bool resolveCollision) {
	this->resolveCollision = resolveCollision;
}
