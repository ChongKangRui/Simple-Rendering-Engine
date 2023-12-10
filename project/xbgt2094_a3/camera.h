#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
protected:
	glm::vec3 position;
	glm::mat4 orient;

	glm::mat4 projection;

	mutable bool isDirty;

	mutable glm::mat4 world;
	mutable glm::mat4 view;

	float fov, zNear, zFar;

public:
	Camera() :
		position(0.0f, 0.0f, 0.0f),
		orient(glm::mat4(1.0)),
		isDirty(true),
		fov(60.0f), zNear(0.1f), zFar(400.0f)
	{ }

	Camera(float fov, float zNear, float zFar) :
		position(0.0f, 0.0f, 0.0f),
		orient(glm::mat4(1.0)),
		isDirty(true),
		fov(fov), zNear(zNear), zFar(zFar)
	{ }

	const glm::vec3 getPosition()
	{
		return position;
	}

	const glm::mat4 getProjectionMatrix()
	{
		return projection;
	}

	void setViewportSize(int viewportWidth, int viewportHeight)
	{
		projection = glm::perspective(glm::radians(fov), (float)viewportWidth / viewportHeight, zNear, zFar);
	}

	void setPosition(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
		isDirty = true;
	}

	void translate(float x, float y, float z)
	{
		position.x += x;
		position.y += y;
		position.z += z;
		isDirty = true;
	}

	void translateLocal(float x, float y, float z)
	{
		glm::vec3 localOrientOffset = orient * glm::vec4(x, y, z, 1.0);
		position += localOrientOffset;
		isDirty = true;
	}

	void rotate(float angle, const glm::vec3& axis)
	{
		orient = glm::rotate(glm::mat4(1.0), glm::radians(angle), axis) * orient;
		isDirty = true;
	}

	void rotateLocal(float angle, const glm::vec3& axis)
	{
		orient = orient * glm::rotate(glm::mat4(1.0), glm::radians(angle), axis);
		isDirty = true;
	}

	inline const glm::mat4& getViewMatrix() const
	{
		if (isDirty)
		{
			world = glm::translate(glm::mat4(1.0), position) * orient;
			view = glm::inverse(world);
			isDirty = false;
		}

		return view;
	}
};
