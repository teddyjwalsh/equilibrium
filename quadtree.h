#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "buffer.h"

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
};

}  // namespace quadtree