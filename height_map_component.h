#pragma once

#include "tuple"

#include "component.h"
#include "quadtree.h"
#include "noise_gen.h"

class CompHeightMap : public Component
{
public:
	int x_size;
	int y_size;
	quadtree::QuadTree quadtree;
	CombinedNoise noise;
	std::vector<float> height_array;
	std::vector<glm::vec4> color_array;
	bool color_changed = true;
	bool height_changed = true;

	CompHeightMap()
	{
		noise.add_noise(5, 2.0);
		noise.add_noise(2, 5.0);
		noise.add_noise(0.5, 20.0);
		noise.add_noise(0.01, 100.0);
	}

	int height_map_index(int x, int y)
	{
		return std::clamp(int(y), 0, y_size - 1) * x_size + std::clamp(int(x), 0, x_size - 1);
	}

	float get(int x, int y)
	{
		return height_array[height_map_index(x, y)];
	}

	glm::vec3 get_normal(int x, int y)
	{
		glm::vec2 r(x, y);
		if (int(floor(r.x)) > x_size - 1 || int(floor(r.x)) < 1 || int(floor(r.y)) > y_size - 1 || int(floor(r.y)) < 1)
		{
			return glm::vec3(0, 0, 1);
		}
		float h1 = get(x-1, y);
		float h2 = get(x+1, y);
		float h3 = get(x, y-1);
		float h4 = get(x, y+1);
		glm::vec3 v1 = glm::normalize(glm::vec3(r.x + 1, r.y, h2) - glm::vec3(r.x - 1, r.y, h1));
		glm::vec3 v2 = glm::normalize(glm::vec3(r.x, r.y + 1, h4) - glm::vec3(r.x, r.y - 1, h3));
		return glm::normalize(glm::cross(v1, v2));
	}
};