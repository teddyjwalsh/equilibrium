#pragma once

#include "component.h"

#include "glm/glm.hpp"

class CompDirectionalLight : public Component
{
public:
	float intensity;
	glm::vec3 direction;
};