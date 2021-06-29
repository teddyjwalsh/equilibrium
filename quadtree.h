#pragma once

#include <vector>
#include <tuple>
#include <algorithm>

#include "glm/glm.hpp"
#include "buffer.h"

inline glm::ivec4 offset_to_sorted_children(glm::vec3 offset)
{
	if (offset.x < 0 && offset.y < 0)
	{
		return glm::ivec4(0, 1, 2, 3);
	}
	if (offset.x > 0 && offset.y < 0)
	{
		return glm::ivec4(1, 3, 0, 2);
	}
	if (offset.x > 0 && offset.y > 0)
	{
		return glm::ivec4(3, 1, 2, 0);
	}
	if (offset.x < 0 && offset.y > 0)
	{
		return glm::ivec4(2, 0, 3, 1);
	}
	return glm::ivec4(0, 1, 2, 3);
}

inline std::tuple<bool, float, float> ray_intersect_aabb(glm::vec3 origin, glm::vec3 dir, glm::vec3 bmin, glm::vec3 bmax)
{
	float tx1 = (bmin.x - origin.x) / dir.x;
	float tx2 = (bmax.x - origin.x) / dir.x;

	float tmin = std::min(tx1, tx2);
	float tmax = std::max(tx1, tx2);

	float ty1 = (bmin.y - origin.y) / dir.y;
	float ty2 = (bmax.y - origin.y) / dir.y;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	float tz1 = (bmin.z - origin.z) / dir.z;
	float tz2 = (bmax.z - origin.z) / dir.z;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	//bool hit = tmax >= tmin;
	return std::make_tuple(tmax >= tmin, tmin, tmax);
}

namespace quadtree
{

inline int vec_to_child(const glm::vec3& in_vec)
{
	return 1 * int(bool(in_vec.x >= 0.5)) + 2 * int(bool(in_vec.y >= 0.5)) + 4 * int(bool(in_vec.z > 0.5));
}

inline glm::vec3 child_to_vec(int child_index)
{
	return glm::vec3(int(bool(child_index & 1)), int(bool(child_index & 2)), int(bool(child_index & 4)));
}

struct QuadTreeNode
{
	QuadTreeNode(const glm::vec3& in_loc, float in_size) :
		location(in_loc),
		size(in_size),
		is_object(false),
		max_height(0)
	{

	}

	unsigned int children[4] = { 0,0,0,0 };
	glm::vec3 location;
	float max_height;
	float size;
	int is_object;
	float pad[2];
};

class QuadTree
{
	std::vector<QuadTreeNode> _nodes;
	std::shared_ptr<bgfx::Buffer<QuadTreeNode>> _buffer;

public:
	QuadTree(float size = 128, glm::vec3 loc = glm::vec3(0))
	{
		_nodes.push_back(QuadTreeNode(loc, size));
		_buffer = std::make_shared<bgfx::Buffer<QuadTreeNode>>();
	}

	void add_node(const glm::vec3& in_loc, float size, float height, int cur_node = 0)
	{
		if (abs(_nodes[cur_node].size - size) < size / 1000.0)
		{
			_nodes[cur_node].is_object = true;
		}
		else
		{
			int child_index = vec_to_child((in_loc - glm::vec3(_nodes[cur_node].location)) / _nodes[cur_node].size);
			if (_nodes[cur_node].children[child_index] == 0)
			{
				auto child_loc = child_to_vec(child_index);
				_nodes.push_back(QuadTreeNode(_nodes[cur_node].location + child_loc*_nodes[cur_node].size / 2.0f, _nodes[cur_node].size / 2.0));
				_nodes[cur_node].children[child_index] = _nodes.size() - 1;
			}
			add_node(in_loc, size, height, _nodes[cur_node].children[child_index]);
		}
		_nodes[cur_node].max_height = std::max(height, _nodes[cur_node].max_height);
	}

	void load_nodes()
	{
		_buffer->set_data(_nodes, bgfx::BindPoint::SHADER_STORAGE_BUFFER);
	}

	std::shared_ptr<bgfx::Buffer<QuadTreeNode>> get_buffer()
	{
		return _buffer;
	}

	std::tuple<bool, glm::vec3> ray_into_height_map_quadtree(const glm::vec3& origin, const glm::vec3& direction, int root=0)
	{
		auto cur_box = std::make_pair(_nodes[root].location, _nodes[root].location + glm::vec3(_nodes[root].size, _nodes[root].size, _nodes[root].max_height));
		auto [hit, tmin, tmax] = ray_intersect_aabb(origin, direction, cur_box.first, cur_box.second);
		if (hit)
		{
			glm::vec3 intersect = (origin + direction * tmin);
			if (_nodes[root].is_object)
			{
				return std::make_tuple(hit, intersect);
			}
			else
			{
				auto sorted_children = offset_to_sorted_children(origin - (_nodes[root].location + _nodes[root].size/2.0f));
				for (int child_index = 0; child_index < 4; ++child_index)
				{
					if (_nodes[root].children[child_index])
					{
						std::tie(hit, intersect) = ray_into_height_map_quadtree(origin, direction, _nodes[root].children[child_index]);
						if (hit)
						{
							return std::make_tuple(hit, intersect);
						}
					}
				}
			}
		}
		return std::make_tuple(false, glm::vec3(0));
	}
};

}  // namespace quadtree