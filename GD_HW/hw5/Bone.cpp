#include "Bone.h"
#include <glm/gtx/quaternion.hpp>

void Bone::rotate(glm::vec3 theta) {
	this->Mi_l = glm::identity<glm::mat4>();
	this->Mi_l = glm::rotate(this->Mi_l, theta.z, glm::vec3(0, 0, 1));
	this->Mi_l = glm::rotate(this->Mi_l, theta.y, glm::vec3(0, 1, 0));
	this->Mi_l = glm::rotate(this->Mi_l, theta.x, glm::vec3(1, 0, 0));
	this->quat = glm::quat_cast(this->Mi_l);
}

void Bone::setQuat(const glm::quat &quat) {
	this->Mi_l = glm::toMat4(quat);
	this->quat = glm::quat_cast(this->Mi_l);
}

void Bone::calc_Mi_d() {
	if (this->parent == nullptr)
		this->Mi_d = glm::identity<glm::mat4>();
	else
		this->Mi_d = this->parent->Mi_d * this->Mi_p;
	for (auto child : this->children)
		child->calc_Mi_d();
}

void Bone::calc_Mi_a() {
	if (this->parent == nullptr)
		this->Mi_a = this->Mi_l;
	else
		this->Mi_a = this->parent->Mi_a * this->Mi_p * this->Mi_l;
	for (auto child : this->children)
		child->calc_Mi_a();
}

void Bone::calc_bone_point(glm::vec3 &p1, glm::vec3 &p2) const {
	auto v1 = glm::vec4(0, 0, 0, 1);
	auto v2 = glm::vec4(this->length, 0, 0, 1);

	v1 = this->Mi_a * v1;
	v2 = this->Mi_a * v2;

	p1 = glm::vec3(v1.x / v1.w, v1.y / v1.w, v1.z / v1.w);
	p2 = glm::vec3(v2.x / v2.w, v2.y / v2.w, v2.z / v2.w);
}


glm::vec3 Bone::transform(const glm::vec3 &vertex) const {
	glm::mat4 Mi = this->Mi_a * glm::inverse(this->Mi_d);
	auto v = Mi * glm::vec4(vertex, 1);
	return glm::vec3(v.x / v.w, v.y / v.w, v.z / v.w);
}

glm::vec3 Bone::transform_from_boneSpace(const glm::vec3 &vertex) const {
	auto v = this->Mi_a * glm::vec4(vertex, 1);
	return glm::vec3(v.x / v.w, v.y / v.w, v.z / v.w);
}

glm::vec3 Bone::transform_from_orig_boneSpace(const glm::vec3 &vertex) const {
	auto v = this->Mi_d * glm::vec4(vertex, 1);
	return glm::vec3(v.x / v.w, v.y / v.w, v.z / v.w);
}

float Bone::getAngle() const {
	return glm::atan(this->Mi_l[0][1], this->Mi_l[0][0]);
}
