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
  }
};
