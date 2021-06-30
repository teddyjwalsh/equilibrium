#pragma once

#include "system.h"

class SysAtmosphere : public System
{
public:
  virtual void init_update() override
  {
  }
  
  virtual void update(double dt) override
  {
    auto& light_component = get_array<CompLightMap>()[0];
    auto& temp_dev_component = get_array<CompTempMap>()[0];
    for (x)
    {
      for (y)
      {
        temp_dev_component.add_to_temp(x, y, light_component.get_light(x,y)*light_to_temp_factor);
      }
    }
    auto& lungs = get_array<CompLung>();
    auto& atmosphere_levels = get_array<CompAtmosphere>();
    for (auto& lung : lungs)
    {
      float oxygen_taken = atmosphere_levels.oxygen*lungs.oxygen_intake*0.001*dt;
      float oxygen_given = std::min(lungs.oxygen_output*dt, lungs.oxygen_tank);
      atmosphere_levels.oxygen += oxygen_given;
      atmosphere_levels.oxygen -= oxygen_taken;
      lungs.oxygen_tank += oxygen_taken;
      lungs.oxygen_tank -= oxygen_given;
      
      float co2_taken = atmosphere_levels.co2*lungs.co2_intake*0.001*dt;
      float co2_given = std::min(lungs.co2_output*dt, lungs.co2_tank);
      atmosphere_levels.co2 += co2_given;
      atmosphere_levels.co2 -= co2_taken;
      lungs.co2_tank += co2_taken;
      lungs.co2_tank -= co2_given;
    }
  }
};
