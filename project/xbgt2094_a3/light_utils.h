#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct PointLightData
{
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	PointLightData(glm::vec3 pos, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc) : position(pos), ambient(amb), diffuse(dif), specular(spc) {};

	float rads = 0.0f;

	void setRotation(float radians)
	{
		rads = radians;
	}

	glm::vec3 getPosition()
	{
		glm::mat4 model = glm::mat4(1.0f);
		return glm::rotate(model, rads, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(position, 1.0f);
	}
};

struct DirectionalLightData
{
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float rads = 0.0f;

	DirectionalLightData(glm::vec3 dir, glm::vec3 amb, glm::vec3 dif, glm::vec3 spc) : direction(dir), ambient(amb), diffuse(dif), specular(spc) {};

	void setRotation(float radians)
	{
		rads = radians;
	}

	glm::vec3 getDirection()
	{
		return glm::rotate(glm::mat4(1.0f), rads, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(direction, 0.0f);
	}
};