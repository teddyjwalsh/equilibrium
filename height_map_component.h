#pragma once

#include "tuple"

#include "component.h"
#include "quadtree.h"

class CompHeightMap : public Component
{
public:
	quadtree::QuadTree quadtree;
	std::vector<float> height_array;
	std::vector<glm::vec4> color_array;


};