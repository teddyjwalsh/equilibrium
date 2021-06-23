#pragma once

#include "renderable_mesh.h"
#include "component.h"

class CompRenderableMesh : public Component
{
public:
	bgfx::RenderableMesh mesh;
};