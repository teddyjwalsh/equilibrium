#pragma once

#include "system.h"
#include "height_map_component.h"
#include "renderable_mesh_component.h"
#include "light_map_component.h"
#include "noise_gen.h"

class SysTerrain : public System
{

virtual void init_update() override
{
	int qt_x_size = 1024;
	int qt_y_size = 1024;
	int block_size = 1.0;
	auto& graphics_comp = get_array<CompGraphics>()[0];
	auto& height_map = get_array<CompHeightMap>()[0];
	auto light_map = height_map.sibling<CompLightMap>();
	height_map.quadtree = quadtree::QuadTree(qt_x_size);

	height_map.x_size = qt_x_size;
	height_map.y_size = qt_y_size;

	height_map.height_array.resize(qt_x_size * qt_y_size);
	height_map.color_array.resize(qt_x_size * qt_y_size);
	light_map->map.resize(qt_x_size * qt_y_size, 0.5);

	double max_height;
	for (int i = 0; i < qt_x_size; i += 1)
	{
		for (int j = 0; j < qt_y_size; j += 1)
		{
			double height = height_map.noise.get_point(i, j);
			float dist_from_center = std::sqrt((i - qt_x_size / 2) * (i - qt_x_size / 2) + (j - qt_y_size / 2) * (j - qt_y_size / 2));
			height += 3 * std::pow(std::sqrt(qt_y_size * qt_y_size / 4 + qt_y_size * qt_y_size / 4) - dist_from_center, 1.7) / 2000.0;
			height_map.quadtree.add_node(glm::vec3(i * block_size, j * block_size, 0), block_size, height);
			height_map.height_array[j * qt_x_size + i] = height;
			height_map.color_array[j * qt_x_size + i] = glm::vec4(1, 1, 1, 1);
			max_height = std::max(max_height, height);
		}
	}

	std::vector<float> vertices((height_map.x_size - 1) * (height_map.y_size - 1) * 6 * 3);
	std::vector<float> vertex_colors((height_map.x_size-1)* (height_map.y_size - 1)*6*4);

	int count = 0;
	int color_count = 0;
	for (int j = 0; j < height_map.y_size-1; ++j)
	{
		for (int i = 0; i < height_map.x_size - 1; ++i)
		{
			for (int k = 0; k < 24; k += 4)
			{
				vertex_colors[color_count + k] = 1.0;
				vertex_colors[color_count + k + 1] = 0.0;
				vertex_colors[color_count + k + 2] = 0.0;
				vertex_colors[color_count + k + 3] = 1.0;
			}
			color_count += 24;

			vertices[count] = i;
			count += 1;
			vertices[count] = j;
			count += 1;
			vertices[count] = height_map.height_array[height_map.height_map_index(i, j)];
			count += 1;

			vertices[count] = i + 1;
			count += 1;
			vertices[count] = j;
			count += 1;
			vertices[count] = height_map.height_array[height_map.height_map_index(i + 1, j)];
			count += 1;

			vertices[count] = i;
			count += 1;
			vertices[count] = j + 1;
			count += 1;
			vertices[count] = height_map.height_array[height_map.height_map_index(i, j + 1)];
			count += 1;

			vertices[count] = i + 1;
			count += 1;
			vertices[count] = j;
			count += 1;
			vertices[count] = height_map.height_array[height_map.height_map_index(i + 1, j)];
			count += 1;

			vertices[count] = i + 1;
			count += 1;
			vertices[count] = j + 1;
			count += 1;
			vertices[count] = height_map.height_array[height_map.height_map_index(i + 1, j + 1)];
			count += 1;

			vertices[count] = i;
			count += 1;
			vertices[count] = j + 1;
			count += 1;
			vertices[count] = height_map.height_array[height_map.height_map_index(i, j + 1)];
			count += 1;


		}
	}

	auto height_map_r_mesh = height_map.sibling<CompRenderableMesh>();
	auto height_map_mesh = std::make_shared<bgfx::Mesh>();
	height_map_mesh->set_vertices(vertices, true);
	height_map_mesh->set_vertex_colors(vertex_colors);
	height_map_r_mesh->mesh.set_mesh(height_map_mesh);
	

	auto height_map_material = std::make_shared<bgfx::Material>();
	auto vcolor_node = height_map_material->vertex_color_node();
	auto fcolor_node = height_map_material->frag_color_node();
	vcolor_node->connect(&fcolor_node->inputs["gl_FragColor"]);
	height_map_material->compile();

	height_map_r_mesh->mesh.set_material(height_map_material);
}

virtual void update(double dt) override
{
}

};