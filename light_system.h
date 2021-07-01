#pragma once

#include <cstdlib>

#include "system.h"
#include "light_map_component.h"
#include "height_map_component.h"
#include "directional_light_component.h"

class SysLight : public System
{
public:
  virtual void init_update() override
  {
      auto& directional_light = get_array<CompDirectionalLight>()[0];
      directional_light.direction = glm::vec3(0, -1, -0.2);
      directional_light.intensity = 1.0;

  }
  
  virtual void update(double dt) override
  {
    auto& height_map_comp = get_array<CompHeightMap>()[0];
    auto& directional_lights = get_array<CompDirectionalLight>();
    auto light_map = height_map_comp.sibling<CompLightMap>();
    auto height_map_r_mesh = height_map_comp.sibling<CompRenderableMesh>();
    auto& directional_light = get_array<CompDirectionalLight>()[0];
    auto& time = get_array<CompTime>()[0];
    directional_light.direction = glm::vec3(0, -cos(time.current_time*0.1)+0.1, -sin(time.current_time*0.1));
    static std::vector<float> vertex_colors((height_map_comp.y_size-1)*(height_map_comp.x_size-1)*6*4);

    int per_frame_division = 6;

    for (int i = 0; i < height_map_comp.x_size; i+=per_frame_division)
    {
        for (int j = 0; j < height_map_comp.y_size; j+=per_frame_division)
        {
            int div_index1 = rand() % per_frame_division;
            int div_index2 = rand() % per_frame_division;
            for (auto& d_light : directional_lights)
            {

                // Shoot a ray from just above the x/y center of each height map entry to each light
                glm::vec3 ray_start(i + div_index1 + 0.5, j + div_index2 + 0.5, height_map_comp.height_array[height_map_comp.height_map_index(i+div_index1, j+div_index2)] + 0.01);
                //bool hit = true;
                auto [hit, intersect] = height_map_comp.quadtree.ray_into_height_map_quadtree(ray_start, -d_light.direction);

                float new_val = 0;
                glm::vec3 norm;
                if (!hit)
                {
                    norm = height_map_comp.get_normal(i + div_index1, j + div_index2);
                    new_val = d_light.intensity * glm::dot(norm, -d_light.direction);
                }
                new_val = glm::max(0.0f, new_val);
                light_map->set_value(i + div_index1, j + div_index2, new_val);
                if (i < height_map_comp.x_size - per_frame_division && j < height_map_comp.y_size - per_frame_division)
                {
                    for (int k = 0; k < 24; k += 4)
                    {
                        vertex_colors[(j+div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i+div_index1) * 6 * 4 + k] = new_val/2.0 + vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + k]/2.0;
                        vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + 1 + k] = new_val/2.0 + vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + 1 + k]/2.0;
                        vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + 2 + k] = new_val/2.0 + vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + 2 + k]/2.0;
                        vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + 4 + k] = new_val/2.0 + vertex_colors[(j + div_index2) * (height_map_comp.x_size - 1) * 6 * 4 + (i + div_index1) * 6 * 4 + 4 + k]/2.0;
                    }
                }

            }
        }
    }
    height_map_r_mesh->mesh.get_mesh()->set_vertex_colors(vertex_colors);
  }
};
