#pragma once

#include <vector>
#include <tuple>
#include <algorithm>
#include <memory>

#include "glm/glm.hpp"
#include "buffer.h"

glm::ivec4 offset_to_sorted_children(const glm::vec3& offset);

bool ray_intersect_aabb(const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& bmin, const glm::vec3& bmax, float& tmin, float& tmax);

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
		bool hit = false;
		float tmin = -1;
		float tmax = 1;
		//auto cur_box = std::make_pair(, ));
		if (glm::clamp(origin, _nodes[root].location, _nodes[root].location + glm::vec3(_nodes[root].size, _nodes[root].size, _nodes[root].max_height)) == origin)
		{
			hit = true;
		}
		else
		{
			hit = ray_intersect_aabb(origin, direction, _nodes[root].location, _nodes[root].location + glm::vec3(_nodes[root].size, _nodes[root].size, _nodes[root].max_height), tmin, tmax);
		}
		if (hit && tmax > 0)
		{
			glm::vec3 intersect = (origin + direction * tmin);
			if (_nodes[root].is_object && tmin > 0)
			{
				return std::make_tuple(hit, intersect);
			}
			else
			{
				auto sorted_children = offset_to_sorted_children(origin - (_nodes[root].location + _nodes[root].size/2.0f));
				for (int child_index = 0; child_index < 4; ++child_index)
				{
					if (_nodes[root].children[sorted_children[child_index]])
					{
						std::tie(hit, intersect) = ray_into_height_map_quadtree(origin, direction, _nodes[root].children[sorted_children[child_index]]);
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