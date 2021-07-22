#pragma once

#include <iostream>
#include "Skeleton.h"

#define KEYFRAME_COUNT 12

struct Vertex {
	float orig_x;
	float orig_y;
	int bone_1;
	int bone_2;
	float weigth_1;
	float weigth_2;
	float x;
	float y;
};

static const char *MODE_NAMES[] = {"Normal", "Inverse", "Review", "Animation"};

class Scene {
public:

	static const auto
	/// free rotate
	MODE_NORMAL = 0,
	/// do inverse kinematic
	MODE_INVERSE = 1,
	/// instance show each keyframe skeleton pos
	MODE_REVIEW = 2,
	/// play keyframes
	MODE_ANIMATION = 3;

	Scene()
			: selectedBone(nullptr), keyframes(KEYFRAME_COUNT), inverse_target(0, 0) {
		skeleton = new Skeleton();
		mode = MODE_NORMAL;
	}

	void AddBone(const std::string &parent, const std::string &bone_name, int length, float angle);

	Bone *GetBone(const std::string &bone_name);

	Bone *GetBone(int bone_index);

	int GetBoneCount();

	void SelectBone(const std::string &bone_name);

	void SelectBone(int bone_index);

	void RotateSelectedBone(float delta_theta);

	Bone *GetSelectedBone();

	void SetSkin(const std::vector<Vertex> &skin);


	void InverseKinematic();

	void Animate(float deltaTime);


	void Init();

	void Update(float deltaTime);

	void Render();


	void SwitchMode() {
		animationTime = 0;
		mode = (mode + 1) % 4;
		std::cout << MODE_NAMES[mode] << " mode" << std::endl;
	};

	void PlayPause();

	void SetKeyFrame(int frame);


	glm::vec2 inverse_target;
	int mode;

private:
	std::vector<Vertex> skin;
	Skeleton *skeleton;
	Bone *selectedBone;

	float animationTime = 0;

	std::vector<std::map<int, glm::quat> > keyframes;
};