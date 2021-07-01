#pragma once

#include "system.h"
#include "movement_component.h"
#include "physics_component.h"
#include "height_map_component.h"

class SysMovement : public System
{
public:
	virtual void init_update() override
	{

	}

	virtual void update(double dt) override
	{
		auto& move_comps = get_array<CompMovement>();
		auto& height_map = get_array<CompHeightMap>()[0];

		for (auto& move_comp : move_comps)
		{
			auto pos_comp = move_comp.sibling<CompPosition>();
			if (pos_comp)
			{
				float height = height_map.get(pos_comp->pos.x, pos_comp->pos.y);
				pos_comp->pos.z = height + 1;
			}
		}
	}
};