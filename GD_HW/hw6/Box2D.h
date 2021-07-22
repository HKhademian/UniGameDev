#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class CollisionType {
	Internal, External
};

enum class BodyType {
	Box, Circle,
};

struct Shape {
	Shape(float mass, float momentOfInertia) :
			mass(mass), momentOfInertia(momentOfInertia) {}

	float mass;
	float momentOfInertia;
};

struct BoxShape : public Shape {
	BoxShape(float h, float w, float mass);

	float width;
	float height;

	std::vector<glm::vec2> vertices;
};

struct CircleShape : public Shape {
	CircleShape(float r, float mass)
			: Shape(mass, mass * r * r),
				radius(r) {}

	float radius;
};


class Body2D {
public:
	explicit Body2D(CollisionType type = CollisionType::External) :
			position(0), linearVelocity(0), force(0),
			angle(0), angularVelocity(0), torque(0),
			collisionType(type) {}

	[[nodiscard]]
	inline virtual Shape &GetShape() = 0;

	[[nodiscard]]
	inline virtual BodyType GetBodyType() const = 0;

	[[nodiscard]]
	inline CollisionType GetCollisionType() const { return this->collisionType; }


	void MoveTo(float x, float y, bool relative = false);

	void SetLinearVelocity(float vx, float vy);

	void SetForce(float fx, float fy);

	void Rotate(float angle, bool relative = false);

	void SetAngularVelocity(float av);

	void SetTorque(float torque);

	[[nodiscard]]
	virtual glm::vec2 GetCentroid() const { return this->position; };

	[[nodiscard]]
	virtual std::vector<glm::vec2> GetVertices(bool lite) const { return std::vector<glm::vec2>(); }

	[[nodiscard]]
	inline glm::vec2 GetPosition() const { return this->position; }

	[[nodiscard]]
	inline glm::vec2 GetLinearVelocity() const { return this->linearVelocity; }

	[[nodiscard]]
	inline glm::vec2 GetForce() const { return this->force; }

	[[nodiscard]]
	inline float GetAngle() const { return this->angle; }

	[[nodiscard]]
	inline float GetAngularVelocity() const { return this->angularVelocity; }

	[[nodiscard]]
	inline float GetTorque() const { return this->torque; }

	inline float GetMass() { return GetShape().mass; }

	void ChangeMass(float mass, bool relative);

	inline float GetMomentOfInertia() { return GetShape().momentOfInertia; }

	float GetAngularMass(glm::vec2 r, glm::vec2 n);

protected:
	glm::vec2 position;
	glm::vec2 linearVelocity;
	glm::vec2 force;

	float angle;
	float angularVelocity;
	float torque;

	CollisionType collisionType;
};


class Box2D : public Body2D {
public:
	Box2D(float h, float w, float mass, CollisionType type = CollisionType::External);

	[[nodiscard]]
	glm::vec2 GetCentroid() const override;

	[[nodiscard]]
	std::vector<glm::vec2> GetVertices(bool lite) const override;


	[[nodiscard]]
	inline Shape &GetShape() override { return shape; }

	[[nodiscard]]
	inline BodyType GetBodyType() const override { return BodyType::Box; }

private:
	BoxShape shape;
};

class Circle2D : public Body2D {
public:
	Circle2D(float radius, float mass, CollisionType type = CollisionType::External) :
			Body2D(type), shape(radius, mass) {}

	[[nodiscard]]
	std::vector<glm::vec2> GetVertices(bool lite) const override;

	[[nodiscard]]
	inline Shape &GetShape() override { return shape; }

	[[nodiscard]]
	inline BodyType GetBodyType() const override { return BodyType::Circle; }

	[[nodiscard]]
	inline float GetRadius() const { return shape.radius; }

private:
	CircleShape shape;

};
