#pragma once

#include <utility>
#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Bone {
	friend class Skeleton;

public:

	Bone(int length, std::string name) :
			length(length),
			name(std::move(name)),
			parent(nullptr),
			Mi_p(1), Mi_d(1),
			Mi_l(1), Mi_a(1),
			t(0), quat(0, 0, 0, 0) {}

	void rotate(glm::vec3 theta);

	void calc_Mi_d();

	void calc_Mi_a();

	void calc_bone_point(glm::vec3 &p1, glm::vec3 &p2) const;


	[[nodiscard]] float getAngle() const;

	void setQuat(const glm::quat &qua);

	[[nodiscard]] glm::quat getQuat() const { return this->quat; }

	[[nodiscard]] glm::vec3 getAxle() const { return glm::axis(this->quat); }

	[[nodiscard]] glm::vec3 transform(const glm::vec3 &vertex) const;

	[[nodiscard]] glm::vec3 transform_from_boneSpace(const glm::vec3 &vertex) const;

	[[nodiscard]] glm::vec3 transform_from_orig_boneSpace(const glm::vec3 &vertex) const;

	[[nodiscard]] inline int getLength() const { return this->length; }

	[[nodiscard]] inline std::string getName() const { return this->name; }

	[[nodiscard]] inline glm::mat4 getLocalTransformation() const { return this->Mi_l; }

	[[nodiscard]] inline Bone *getParent() const { return this->parent; }

private:

	Bone *parent;
	std::vector<Bone *> children;

	std::string name;
	int length;

	glm::mat4 Mi_p; // boneSpace to parentSpace
	glm::mat4 Mi_d; // boneSpace to charSpace   					=>   Mi,dInv: charSpace(defPos) to boneSpace(defPos)
	glm::mat4 Mi_l; // boneSpace transform (in animation)
	glm::mat4 Mi_a; // charSpace transform (animation)

	// Key Data
	glm::vec3 t;
	glm::quat quat;
};