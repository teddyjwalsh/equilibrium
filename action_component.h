#pragma once

#include <deque>

#include "component.h"

struct Action
{
  enum class ActionType
  {
    MOVE,
    USE_TOOL,
    USE_PLACED_OBJECT
  };
  
  ActionType action_type;
  glm::vec2 location;
  EntityId use_on;
  EntityId tool;
};

class CompAction : public Component
{
  std::deque<Action> actions;
  Action pop() { Action out_action = actions.back(); actions.pop_back(); }
  void push(const Action& in_action) { actions.push_front(in_action); }
};
