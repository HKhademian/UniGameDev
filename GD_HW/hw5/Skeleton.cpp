#include "Skeleton.h"

void Skeleton::addBone(Bone *parent, Bone *bone, glm::vec3 translate, glm::vec3 rotation) {
	if (parent == nullptr) this->root = bone;
	else {
		parent->children.push_back(bone);
		bone->parent = parent;

		bone->Mi_p = glm::identity<glm::mat4>();
		bone->Mi_p = glm::translate(bone->Mi_p, glm::vec3(parent->length, 0, 0));
		bone->Mi_p = glm::rotate(bone->Mi_p, rotation.z, glm::vec3(0, 0, 1));
		bone->Mi_p = glm::rotate(bone->Mi_p, rotation.y, glm::vec3(0, 1, 0));
		bone->Mi_p = glm::rotate(bone->Mi_p, rotation.x, glm::vec3(1, 0, 0));
		bone->Mi_p = glm::translate(bone->Mi_p, translate);
	}

	this->BoneNames.push_back(bone->name);
	this->Bones[bone->name] = bone;
}

std::vector<std::string> Skeleton::getBoneNames() {
	return BoneNames;
}

Bone *Skeleton::getBone(const std::string &bone_name) {
	if (this->Bones.count(bone_name) != 0) return this->Bones[bone_name];
	else return nullptr;
}

void Skeleton::calc_Mi_d() {
	this->root->calc_Mi_d();
}

void Skeleton::calc_Mi_a() {
	this->root->calc_Mi_a();
}