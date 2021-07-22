#include <vector>
#include <glm/vec2.hpp>

void SegmentVertices(std::vector<glm::vec2> &vertices, int segs, float radius, float angle, glm::vec2 pos) {
	const auto rm = glm::rotate(glm::mat4(1), glm::radians(angle), glm::vec3(0, 0, 1));

	vertices.clear();
	vertices.emplace_back(pos.x, pos.y);
	for (int ii = 0; ii < segs + 1; ii++) {
		float theta = 2.0f * glm::pi<float>() * float(ii) / float(segs);//get the current angle
		float x = radius * glm::cos(theta); //calculate the x component
		float y = radius * glm::sin(theta); //calculate the y component
		auto v = glm::vec2(rm * glm::vec4(x, y, 0, 1)) + pos;
		vertices.push_back(v); //output vertex
	}
}

std::vector<glm::vec2> CircleVertices(float radius, float angle, glm::vec2 pos) {
	const auto segs = 10 + int(radius / 5);
	std::vector<glm::vec2> vertices;
	SegmentVertices(vertices, segs, radius, angle, pos);
	return vertices;
}

struct Object {
	int segments = 5;
	float radius = 5;
	float angle = 0;
	glm::vec2 position;
	bool visible = false;

	std::vector<glm::vec2> vertices;

	void GenerateVertices();
};

void Object::GenerateVertices() {
	::SegmentVertices(vertices, segments, radius, angle, position);
}
