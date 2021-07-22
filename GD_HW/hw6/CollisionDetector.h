#pragma once

#include "Box2D.h"
#include <tuple>

struct CollisionInfo {
	Body2D *body1;
	Body2D *body2;
	glm::vec2 penetrationPoint;
	glm::vec2 contactPoint;
	glm::vec2 normal;
	float penetrationDepth;
	bool isCollided;
};

class CollisionDetector {
public:
	static bool ShapeContainsPoint(Body2D *body, glm::vec2 p);

	static glm::vec2 ProjectToEdge(glm::vec2 v1, glm::vec2 v2, glm::vec2 p);

	static glm::vec2 GetEdgeNormal(glm::vec2 edge);

	static bool IsPointBetweenTwoPoint(glm::vec2 v1, glm::vec2 v2, glm::vec2 p);

	CollisionInfo Detect(Body2D *body1, Body2D *body2) const;

	CollisionInfo Detect(Body2D *body1, Circle2D *body2) const;

	CollisionInfo Detect(Circle2D *body1, Body2D *body2) const;

	CollisionInfo Detect(Circle2D *body1, Circle2D *body2) const;

	std::vector<CollisionInfo> Detect(std::vector<Body2D *> *bodies) const;
};