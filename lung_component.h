#pragma once

#include "component.h"

class CompLung : public Component
{
public:
  float oxygen_intake;
  float co2_intake;
  float oxygen_tank;
  float co2_tank;
};
