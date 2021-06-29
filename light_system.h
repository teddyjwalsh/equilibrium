#pragma once

class SysLight : public System
{
public:
  virtual void init_update() override
  {
  }
  
  virtual void update(double dt) override
  {
    auto& height_map_comp = get_array<CompHeightMap>()[0];
    auto& directional_lights = get_array<CompDirectionalLight>();
    auto& light_map = height_map_comp.sibling<CompLightMap>();
    auto& height_map_r_mesh = height_map_comp.sibling<CompRenderableMesh>();
    for (int i = 0; i < height_map_comp.x_size; ++i)
    {
      for (int j = 0; j < height_map_comp.y_size; ++j)
      {
        for (auto& d_light : directional_lights)
        {
          // Shoot a ray from just above the x/y center of each height map entry to each light
          glm::vec3 ray_start(i+0.5, j+0.5, height_map_comp.height_array[height_map_comp.height_map_index(i, j)] + 0.01);
          auto& [hit, intersect] = height_map_comp.quadtree.shoot_ray(ray_start, -d_light.direction);
          float new_val = 0;
          if (!hit)
          {
            glm::vec3 norm = height_map_comp.get_normal(i, j);
            float new_val = d_light.intensity*glm::dot(norm, -d_light.direction);
          }
          light_map.light_array.set_val(i, j, glm::max(0, new_val);
          height_map_r_mesh.set_vertex_color(i, j, glm::vec3(1)*new_val);
        }
      }
    }
  }
};
