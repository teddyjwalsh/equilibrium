#pragma once

#include "tuple"

#include "component.h"
#include "quadtree.h"

class CompHeightMap : public Component
{
public:
	quadtree::QuadTree quadtree;


};