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
      Action& cur_action = action_comp.back();
      switch(cur_action.action_type)
      {
      case Action::ActionType::MOVE:
        break;
      default:
        break;
      }
    }
  }
}
