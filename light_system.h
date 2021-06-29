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
    for (int i = 0; i < height_map_comp.x_size; ++i)
    {
      for (int j = 0; j < height_map_comp.y_size; ++j)
      {
        for (auto& d_light : directional_lights)
        {
          // Shoot a ray from just above the x/y center of each height map entry to each light
          glm::vec3 ray_start(i+0.5, j+0.5, height_map_comp.height_array[height_map_comp.height_map_index(i, j)] + 0.01);
          auto& [hit, intersect] = height_map_comp.quadtree.shoot_ray(ray_start, -d_light.direction);
          if (!hit)
          {
            glm::vec3 norm = height_map_comp.get_normal(i, j);
            light_map.light_array.set_val(i, j, glm::max(0, d_light.intensity*glm::dot(norm, -d_light.direction));
          }
        }
      }
    }
  }
};
