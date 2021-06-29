#pragma once
#include "component.h"
#include "glm/glm.hpp"

#include "camera.h"

class CompCamera :
    public Component
{
public:
    bgfx::Camera camera;

    CompCamera() : camera(1000, 800) {}

};

