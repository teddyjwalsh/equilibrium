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
          if (dist_vec.length() > 2)
          {
            glm::vec3 direction = glm::normalize();
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
