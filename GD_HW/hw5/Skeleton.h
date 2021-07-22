#pragma once

#include <map>
#include "Bone.h"

class Skeleton {
public:

	void addBone(Bone *parent, Bone *bone, glm::vec3 translate, glm::vec3 rotation);

	std::vector<std::string> getBoneNames();

	Bone *getBone(const std::string &bone_name);

	void calc_Mi_d();

	void calc_Mi_a();

private:

	Bone *root;
	std::vector<std::string> BoneNames;
	std::map<std::string, Bone *> Bones;
};