#pragma once
#include "Box2D.h"
#include "CollisionDetector.h"

class CollisionResolver
{
public:
	void Resolve(CollisionInfo& info);
	void Resolve(std::vector<CollisionInfo>& info);

	void ResolveAngular(CollisionInfo& info);
};