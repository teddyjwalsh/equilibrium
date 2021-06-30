#pragma once

#include <vector>

#include "component.h"

class CompLightMap : public Component
{
public:
	int x_size;
	int y_size;
	std::vector<float> map;
	void set_value(int x, int y, float in_val)
	{
		map[y * x_size + x] = in_val;
	}
};
