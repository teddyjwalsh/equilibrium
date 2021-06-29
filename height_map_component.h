#pragma once

#include "tuple"

#include "component.h"
#include "quadtree.h"

class CompHeightMap : public Component
{
public:
	int x_size;
	int y_size;
	quadtree::QuadTree quadtree;
	std::vector<float> height_array;
	std::vector<glm::vec4> color_array;
	bool color_changed = true;
	bool height_changed = true;

	int height_map_index(int x, int y)
	{
		return std::clamp(int(y), 0, y_size - 1) * x_size + std::clamp(int(x), 0, x_size - 1);
	}
};