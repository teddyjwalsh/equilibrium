#pragma once

#include "system.h"

class SysAction : public System
{
public:
  virtual void init_update() override
  {
    
  }
  
  virtual void update(double dt) override
  {
    auto& action_comps = get_array<CompAction>();
    auto& height_map = get_array<CompHeightMap>()[0];
    for (auto& action_comp : action_comps)
    {
      auto movement_comp = action_comp.sibling<CompMovement>();
      auto position_comp = action_comp.sibling<CompPosition>();
      Action& cur_action = action_comp.queue.back();
      switch(cur_action.action_type)
      {
      case Action::ActionType::MOVE:
        if (position_comp && movement_comp)
        {
          auto dist_vec = cur_action.location - position_comp.pos;
          dist_vec.z = 0;
          if (dist_vec.length() > 2)
          {
            glm::vec3 direction = glm::normalize();
            // Check here for height of movement, account for this in movement speed and energy used
            position_comp.pos += direction*movement_comp.max_speed;
          }
          else
          {
            action_comp.queue.pop_back();
          }
        }
        break;
      default:
        break;
      }
    }
  }
}
