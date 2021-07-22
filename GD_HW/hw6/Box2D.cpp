#include "Box2D.h"

BoxShape::BoxShape(float h, float w, float mass)
		: Shape(mass, mass * (w * w + h * h) / 12), height(h), width(w) {
	vertices.push_back(glm::vec2(-w / 2, -h / 2));
	vertices.push_back(glm::vec2(w / 2, -h / 2));
	vertices.push_back(glm::vec2(w / 2, h / 2));
	vertices.push_back(glm::vec2(-w / 2, h / 2));
}

void Body2D::ChangeMass(float mass, bool relative) {
	if (relative) GetShape().mass += mass;
	else GetShape().mass = mass;
	if (GetShape().mass < 1) GetShape().mass = 1;
}

float Body2D::GetAngularMass(glm::vec2 r, glm::vec2 n) {
	auto c = glm::cross(glm::vec3(r, 0), glm::vec3(n, 0)).z;
	auto m = GetShape().momentOfInertia / c * c;
	return m;
}

void Body2D::MoveTo(float x, float y, bool relative) {
	if (relative) {
		position.x += x;
		position.y += y;
	} else {
		position.x = x;
		position.y = y;
	}
}

void Body2D::SetLinearVelocity(float vx, float vy) {
	linearVelocity.x = vx;
	linearVelocity.y = vy;
}

void Body2D::SetForce(float fx, float fy) {
	force.x = fx;
	force.y = fy;
}

void Body2D::Rotate(float angle, bool relative) {
	if (relative) this->angle += angle;
	else this->angle = angle;
}

void Body2D::SetAngularVelocity(float av) {
	angularVelocity = av;
}

void Body2D::SetTorque(float torque) {
	this->torque = torque;
}


Box2D::Box2D(float h, float w, float mass, CollisionType type) :
		Body2D(type), shape(h, w, mass) {}

std::vector<glm::vec2> Box2D::GetVertices(bool lite) const {
	std::vector<glm::vec2> result;

	auto rm = glm::rotate(glm::mat4(1), this->angle, glm::vec3(0, 0, 1));
	for (auto v : this->shape.vertices) {
		auto nv = rm * glm::vec4(v, 0, 1);
		result.emplace_back(nv.x + position.x, nv.y + position.y);
	}

	return result;
}

glm::vec2 Box2D::GetCentroid() const {
	glm::vec2 centroid(0);
	auto vertices = GetVertices(true);
	for (auto v : vertices) centroid += v;
	return centroid / (float) vertices.size();
}

std::vector<glm::vec2> Circle2D::GetVertices(bool lite) const {
	const auto segs = lite ? 10 : int(10 * shape.radius);
	std::vector<glm::vec2> result;


	auto rm = glm::rotate(glm::mat4(1), this->angle, glm::vec3(0, 0, 1));

	if (!lite && collisionType != CollisionType::Internal)
		result.emplace_back(position.x, position.y);

	for (int ii = 0; ii < segs + (lite ? 0 : 1); ii++) {
		float theta = 2.0f * glm::pi<float>() * float(ii) / float(segs);//get the current angle
		float x = shape.radius * glm::cos(theta); //calculate the x component
		float y = shape.radius * glm::sin(theta); //calculate the y component
		auto v = glm::vec2(rm * glm::vec4(x, y, 0, 1)) + position;
		result.push_back(v); //output vertex
	}

	return result;
}
