#pragma once

#include "glm/glm.hpp"

#include "component.h"

class CompRayCamera : public Component
{
public:
	glm::vec3 location;
	glm::vec3 look;
	glm::vec3 right;
	glm::vec3 up;
	float f;
	float width;
	float height;

	void set_look(const glm::vec3& in_look)
	{
		look = in_look;
		right = glm::normalize(glm::cross(look, glm::vec3(0, 0, 1)));
		up = glm::normalize(glm::cross(right, look));
	}
};