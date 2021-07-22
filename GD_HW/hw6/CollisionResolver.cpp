#include "CollisionResolver.h"

void CollisionResolver::Resolve(CollisionInfo &info) {
	if (info.isCollided) {
		ResolveAngular(info);

		auto v = info.contactPoint - info.penetrationPoint;
		auto r = info.body2->GetMass() / (info.body1->GetMass() + info.body2->GetMass());
		info.body1->MoveTo(-v.x * r, -v.y * r, true);
		info.body2->MoveTo(v.x * (1 - r), v.y * (1 - r), true);
	}
}

void CollisionResolver::Resolve(std::vector<CollisionInfo> &info) {
	for (auto collisionInfo : info) {
		Resolve(collisionInfo);
	}
}

float angleBetween(glm::vec2 a, glm::vec2 b) {
	glm::vec2 da = glm::normalize(a);
	glm::vec2 db = glm::normalize(b);

	auto c = glm::cross(glm::vec3(a, 0), glm::vec3(b, 0)).z >= 0 ? -1.0f : 1.0f;
	auto angle = c * glm::acos(glm::max(0.0f, glm::min(1.0f, glm::dot(da, db))));
	return angle;
}

void CollisionResolver::ResolveAngular(CollisionInfo &info) {
	auto c1 = info.body1->GetCentroid();
	auto c2 = info.body2->GetCentroid();

	auto r1 = info.penetrationPoint - c1;
	auto r2 = info.penetrationPoint - c2;

	auto p1 = info.contactPoint - c1;
	auto p2 = info.contactPoint - c2;

	auto alpha1 = angleBetween(r1, p1);
	auto alpha2 = angleBetween(r2, p2);

	if (info.body1->GetCollisionType() != CollisionType::Internal) info.body1->Rotate(alpha1, true);
	if (info.body2->GetCollisionType() != CollisionType::Internal) info.body2->Rotate(alpha2, true);
}