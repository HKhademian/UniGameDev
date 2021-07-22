#include "CollisionDetector.h"

bool CollisionDetector::ShapeContainsPoint(Body2D *body, glm::vec2 v) {
	if (body->GetBodyType() == BodyType::Circle) {
		return glm::length(v - body->GetCentroid()) < ((Circle2D *) body)->GetRadius();
	}

	auto vertices = ((Box2D *) body)->GetVertices(true);
	auto n1 = vertices.size();

	for (int i = 0; i < n1; i++) {
		auto a = vertices[i];
		auto b = vertices[(i + 1) % n1];

		auto n = GetEdgeNormal(a - b);

		bool isShapeInFrontOfEdge = true;

		if (glm::dot(v - a, n) > 0)
			return false;
	}

	return true;
}

glm::vec2 CollisionDetector::ProjectToEdge(glm::vec2 v1, glm::vec2 v2, glm::vec2 p) {
	auto e1 = v2 - v1;
	auto e2 = p - v1;
	float valDp = glm::dot(e1, e2);
	float lenE1 = glm::length(e1);
	float lenE2 = glm::length(e2);
	float cos = valDp / (lenE1 * lenE2);
	float projLenOfLine = cos * lenE2;
	glm::vec2 projPoint = v1 + e1 / lenE1 * projLenOfLine;
	return projPoint;
}

glm::vec2 CollisionDetector::GetEdgeNormal(glm::vec2 edge) {
	edge /= glm::length(edge);
	auto n = glm::cross(glm::vec3(0, 0, 1), glm::vec3(edge, 0));
	return glm::vec2(n.x, n.y);
}

bool CollisionDetector::IsPointBetweenTwoPoint(glm::vec2 v1, glm::vec2 v2, glm::vec2 p) {
	auto e1 = v2 - v1;
	auto e2 = p - v1;
	auto e3 = p - v2;

	return glm::dot(e1, e2) > 0 && glm::dot(e1, e3) < 0;
}

CollisionInfo CollisionDetector::Detect(Body2D *body1, Circle2D *body2) const {
	CollisionInfo info({body1, body2, glm::vec2(0), glm::vec2(0), glm::vec2(0), 0, false});

	auto body1_vertices = body1->GetVertices(true);
	auto n1 = body1_vertices.size();

	auto r2 = body2->GetRadius();
	auto c2 = body2->GetPosition();

	bool isInternal = body1->GetCollisionType() == CollisionType::Internal;
	if (isInternal)
		return Detect((Body2D *) body1, (Body2D *) body2); //FIXME

	float minDepth = 0;
	bool isShapeInFrontOfEdge = true;

	for (int i = 0; i < n1; i++) {
		auto a = body1_vertices[i];
		auto b = body1_vertices[(i + 1) % n1];
		auto e = b - a;
		auto p = ProjectToEdge(a, b, c2);

		if (IsPointBetweenTwoPoint(a, b, p)) {
			auto penetration = c2 + r2 * glm::normalize(p - c2);
			auto depth = glm::length(penetration - p);

			if (glm::dot(penetration - a, GetEdgeNormal(a - b)) < 0) {
				isShapeInFrontOfEdge = false;

				if (minDepth == 0 || depth < minDepth) {
					info = CollisionInfo({body1, body2, penetration, p, glm::vec2(0), depth, true});
					minDepth = depth;
				}
			}
		}
	}

	if (isShapeInFrontOfEdge && !isInternal) {
		info.isCollided = false;
		return info;
	}

	return info;
}


CollisionInfo CollisionDetector::Detect(Circle2D *body1, Body2D *body2) const {
	CollisionInfo info({body1, body2, glm::vec2(0), glm::vec2(0), glm::vec2(0), 0, false});

	auto body2_vertices = body2->GetVertices(true);
	auto n2 = body2_vertices.size();

	auto r1 = body1->GetRadius();
	auto c1 = body1->GetPosition();

	bool isInternal = body1->GetCollisionType() == CollisionType::Internal;
	if (isInternal)
		return Detect((Body2D *) body1, (Body2D *) body2); //FIXME

	float minDepth = 0;
	bool isShapeInFrontOfEdge = true;

	for (int i = 0; i < n2; i++) {
		auto v = body2_vertices[i];
		auto diff = v - c1;
		auto dist = glm::length(diff);
		auto dir = glm::normalize(diff);

		if (dist < r1) {
			auto p = v;
			auto contactPoint = c1 + r1 * dir;
			auto depth = glm::length(contactPoint - p);
			return CollisionInfo({body1, body2, p, contactPoint, glm::vec2(0), depth, true});
		}
	}

	if (isShapeInFrontOfEdge && !isInternal) {
		info.isCollided = false;
		return info;
	}

	return info;
}

CollisionInfo CollisionDetector::Detect(Circle2D *body1, Circle2D *body2) const {
	CollisionInfo info({body1, body2, glm::vec2(0), glm::vec2(0), glm::vec2(0), 0, false});

	auto r1 = body1->GetRadius();
	auto r2 = body2->GetRadius();

	auto c1 = body1->GetPosition();
	auto c2 = body2->GetPosition();

	bool isInternal = body1->GetCollisionType() == CollisionType::Internal;

	auto diff = c2 - c1;
	auto dist = glm::length(diff);
	auto dir = glm::normalize(diff);

	if (!isInternal) {
		if (dist < r1 + r2) { // external collision detected
			auto depth = (r1 + r2 - dist);
			auto contactPoint = c2 - (r2 - depth) * dir;
			auto penetrationPoint = c1 + (r1 - depth) * dir;
			return CollisionInfo{body1, body2, penetrationPoint, contactPoint, dir, depth, true};
		}
	} else {
		if (dist > r1 - r2) { // external collision detected
			auto depth = dist - (r1 - r2);
			auto contactPoint = c1 + r1 * dir;
			auto penetrationPoint = c1 + (r1 + depth) * dir;
			return CollisionInfo{body1, body2, penetrationPoint, contactPoint, dir, depth, true};
		}
	}
	return info;
}

CollisionInfo CollisionDetector::Detect(Body2D *body1, Body2D *body2) const {
	CollisionInfo info({body1, body2, glm::vec2(0), glm::vec2(0), glm::vec2(0), 0, false});

	auto body1_vertices = body1->GetVertices(true);
	auto body2_vertices = body2->GetVertices(true);

	auto n1 = body1_vertices.size();
	auto n2 = body2_vertices.size();

	float minPenetration = 1e10;
	bool isInternal = body1->GetCollisionType() == CollisionType::Internal;

	bool isShapeInFrontOfEdge = true;

	for (int i = 0; i < n1; i++) {
		auto a = body1_vertices[i];
		auto b = body1_vertices[(i + 1) % n1];

		auto n = GetEdgeNormal(a - b);
		if (isInternal) n = -n;
		else isShapeInFrontOfEdge = true;

		float maxPenetration = 0;
		CollisionInfo max_info({body1, body2, glm::vec2(0), glm::vec2(0), glm::vec2(0), 0, false});

		for (int j = 0; j < n2; j++) {
			auto v = body2_vertices[j];

			if (glm::dot(v - a, n) < 0) {
				isShapeInFrontOfEdge = false;

				auto p = ProjectToEdge(a, b, v);
//				if (IsPointBetweenTwoPoint(a, b, p))
				{
					auto penetration = glm::length(p - v);

					if (penetration > maxPenetration) {
						maxPenetration = penetration;
						max_info = CollisionInfo({body1, body2, v, p, n, penetration, true});
					}
				}
			}
		}

		if (isShapeInFrontOfEdge && !isInternal) {
			info.isCollided = false;
			return info;
		} else if (max_info.isCollided) {
			if (maxPenetration < minPenetration) {
				minPenetration = maxPenetration;
				info = max_info;
			}
		}
	}

	if (isShapeInFrontOfEdge) info.isCollided = false;
	return info;
}

std::vector<CollisionInfo> CollisionDetector::Detect(std::vector<Body2D *> *bodies) const {
	static const float EPS = 1e-5;

	std::vector<CollisionInfo> result;

	if (bodies->size() < 2) return result;

	for (int i = 0; i < bodies->size(); i++) {
		auto body1 = (*bodies)[i];
		for (int j = i + 1; j < bodies->size(); j++) {
			auto body2 = (*bodies)[j];

			CollisionInfo collision_info_1;
			CollisionInfo collision_info_2;

			if (body1->GetBodyType() == BodyType::Circle && body2->GetBodyType() == BodyType::Circle) {
				collision_info_1 = Detect((Circle2D *) body1, (Circle2D *) body2);
				collision_info_2 = Detect((Circle2D *) body2, (Circle2D *) body1);
//			} else if (body1->GetBodyType() == BodyType::Box) {
//				collision_info_1 = Detect((Box2D *) body1, (Circle2D *) body2);
//				collision_info_2 = Detect((Circle2D *) body2, (Box2D *) body1);
//			} else if (body2->GetBodyType() == BodyType::Box) {
//				collision_info_1 = Detect((Circle2D *) body1, (Box2D *) body2);
//				collision_info_2 = Detect((Box2D *) body2, (Circle2D *) body1);
			} else {
				collision_info_1 = Detect((Box2D *) body1, (Box2D *) body2);
				collision_info_2 = Detect((Box2D *) body2, (Box2D *) body1);
			}

			bool isInternal = collision_info_1.body1->GetCollisionType() == CollisionType::Internal;

			if (collision_info_1.isCollided && (isInternal || collision_info_2.isCollided)) {
				auto pd1 = collision_info_1.penetrationDepth;
				auto pd2 = isInternal ? 1E6f : collision_info_2.penetrationDepth;

				if (glm::min(pd1, pd2) < EPS) continue;

				if (pd1 < pd2)
					result.push_back(collision_info_1);
				else
					result.push_back(collision_info_2);
			}
		}
	}

	return result;
}
