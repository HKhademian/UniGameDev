#include <GL/glew.h>
#include <iostream>

#include "Scene.h"

void Scene::AddBone(const std::string &parent_name, const std::string &bone_name, int length, float angle) {
	Bone *parent = GetBone(parent_name);
	Bone *child = new Bone(length, bone_name);

	this->skeleton->addBone(parent, child, glm::vec3(), glm::vec3(0, 0, angle));
}

Bone *Scene::GetBone(const std::string &bone_name) {
	return this->skeleton->getBone(bone_name);
}

Bone *Scene::GetBone(int bone_index) {
	auto bone_names = this->skeleton->getBoneNames();
	return this->GetBone(bone_names[bone_index]);
}

int Scene::GetBoneCount() {
	return (int) this->skeleton->getBoneNames().size();
}

void Scene::SelectBone(const std::string &bone_name) {
	Bone *bone = GetBone(bone_name);
	if (bone != nullptr) this->selectedBone = bone;
}

void Scene::SelectBone(int bone_index) {
	Bone *bone = GetBone(bone_index);
	if (bone != nullptr) this->selectedBone = bone;
}

void Scene::RotateSelectedBone(float delta_theta) {
	if (selectedBone != nullptr)
		selectedBone->rotate(glm::vec3(0, 0, delta_theta));
}

Bone *Scene::GetSelectedBone() {
	return selectedBone;
}

void Scene::SetSkin(const std::vector<Vertex> &newSkin) {
	this->skin = newSkin;
}

void Scene::InverseKinematic() {
	const glm::vec2 &target2 = this->inverse_target;

	const auto target = glm::vec3(target2, 0);
	const auto boneCount = GetBoneCount();

	const auto speed = 1.0f; // to make smooth movement

	Bone *endEffector = GetBone(boneCount - 1);
	Bone *startBone;
	glm::vec3 startBonePos, endEffectorPos;

	bool reach = false;
	int tries = 10;
	while (!reach && tries-- > 0) {
		startBone = endEffector;

		while (startBone != nullptr) {
			startBonePos = startBone->transform_from_boneSpace(glm::vec3(0, 0, 0));
			endEffectorPos = endEffector->transform_from_boneSpace(glm::vec3(endEffector->getLength(), 0, 0));

			if (endEffectorPos.x == target.x && endEffectorPos.y == target.y) {
				reach = true;
				break;
			}

			glm::vec3 U = endEffectorPos - startBonePos;
			glm::vec3 F = target - endEffectorPos;
			glm::vec3 G = target - startBonePos;
			auto u = glm::length(U);
			auto f = glm::length(F);
			auto g = glm::length(G);

			auto tetha = glm::acos((u * u + g * g - f * f) / (2 * u * g));

			auto prevAngle = startBone->getAngle();; // glm::angle(startBone->getQuat());


			// https://stackoverflow.com/questions/13221873/
			float UdotG =  U.x * -G.y + U.y * G.x;

			float newAngle = prevAngle;
			if (UdotG > 0) // G right of U
				newAngle -= tetha * speed;
			else if (UdotG < 0) // G left of U
				newAngle += tetha * speed;

			if (!isnanf(newAngle) && !isnanf(tetha) && glm::abs(tetha) > 0.001) {
				startBone->rotate(glm::vec3(0, 0, newAngle));
				startBone->calc_Mi_a();
			}

			startBone = startBone->getParent();
		}
	}
}


static glm::quat operator*(float f, const glm::quat &q) {
	return glm::quat_cast(f * glm::mat4_cast(q));
}

static glm::quat slerp(const glm::quat &q1, const glm::quat &q2, float u) {
	return glm::slerp(q1, q2, u);
	auto teta = glm::dot(q1, q2) / (glm::length(q1) * glm::length(q2));
	return float(glm::sin((1 - u) * teta) / sin(teta)) * q1 + float(glm::sin(u * teta) / sin(teta)) * q2;
}

void Scene::Animate(float deltaTime) {
	const auto boneCount = GetBoneCount();

	animationTime += deltaTime;

	float progress = animationTime - float(int(animationTime));
	int startFrameIndex = int(animationTime) % KEYFRAME_COUNT;
	int endFrameIndex = (startFrameIndex + 1) % KEYFRAME_COUNT;

	auto &startFrame = keyframes[startFrameIndex];
	auto &endFrame = keyframes[endFrameIndex];

	auto defaultQuat = glm::quat_cast(glm::identity<glm::mat4>());

	for (auto i = 0; i < boneCount; i++) {
		auto startQuat = startFrame.contains(i) ? startFrame[i] : defaultQuat;
		auto endQuat = endFrame.contains(i) ? endFrame[i] : defaultQuat;
		auto quat = slerp(startQuat, endQuat, progress);
		GetBone(i)->setQuat(quat);
	}
}


void Scene::PlayPause() {}

void Scene::SetKeyFrame(int frame) {
	if (frame < 0 || frame >= KEYFRAME_COUNT) return;

	const auto boneCount = GetBoneCount();

	if (mode == MODE_ANIMATION) {
		animationTime = float(frame);
	} else if (mode == MODE_REVIEW) {
		auto &keyframe = keyframes[frame];
		auto defaultQuat = glm::quat_cast(glm::identity<glm::mat4>());
		for (auto i = 0; i < boneCount; i++) {
			GetBone(i)->setQuat(keyframe.contains(i) ? keyframe[i] : defaultQuat);
		}
	} else {
		for (auto i = 0; i < boneCount; i++) {
			keyframes[frame].insert_or_assign(i, GetBone(i)->getQuat());
		}
	}
}


void Scene::Init() {
	this->skeleton->calc_Mi_d();
}

void Scene::Update(float deltaTime) {
	if (mode == MODE_INVERSE) {
		InverseKinematic();
	} else if (mode == MODE_ANIMATION) {
		Animate(deltaTime);
	}

	this->skeleton->calc_Mi_a();

	for (Vertex &vert : skin) {
		auto bone1 = GetBone(vert.bone_1);
		auto bone2 = GetBone(vert.bone_2);

		auto orig_pos = glm::vec3(vert.orig_x, vert.orig_y, 1);
		auto pos1 = bone1->transform(orig_pos);
		auto pos2 = bone2->transform(orig_pos);

		vert.x = pos1.x * vert.weigth_1 + pos2.x * vert.weigth_2;
		vert.y = pos1.y * vert.weigth_1 + pos2.y * vert.weigth_2;
	}
}

void Scene::Render() {
	// Draw Origin
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2f(-20, 0);
	glVertex2f(20, 0);
	glVertex2f(0, -20);
	glVertex2f(0, 20);
	glEnd();

	// Draw Skeleton
	auto bone_names = this->skeleton->getBoneNames();
	for (const auto &bone_name : bone_names) {
		glm::vec3 bone_start_point, bone_end_point, top_point(2, 2, 0), bottom_point(2, -2, 0);

		auto bone = GetBone(bone_name);

		bone->calc_bone_point(bone_start_point, bone_end_point);
		top_point = bone->transform_from_boneSpace(top_point);
		bottom_point = bone->transform_from_boneSpace(bottom_point);

		if (bone == this->selectedBone) glLineWidth(4);
		glColor3f(0, 1, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2f(bone_start_point.x, bone_start_point.y);
		glVertex2f(top_point.x, top_point.y);
		glVertex2f(bone_end_point.x, bone_end_point.y);
		glVertex2f(bottom_point.x, bottom_point.y);
		glEnd();
		glLineWidth(1);
	}

	// Draw Skin
	glColor3f(0, 0, 1);
	glBegin(GL_POINTS);
	for (auto vert : this->skin) glVertex2f(vert.x, vert.y);
	glEnd();
}