#pragma once
#include <string>
#include <glm/vec2.hpp>

struct SimpleMesh
{
	unsigned int VAO;
	unsigned int indices;
	void draw();
};

namespace MeshUtils
{
	SimpleMesh* makePlane(glm::vec2 size, glm::ivec2 partitions, glm::ivec2 tiling);
	SimpleMesh* simpleObjLoader(const std::string& filePath);
	SimpleMesh* makeQuad();
	SimpleMesh* makeSkybox();
}