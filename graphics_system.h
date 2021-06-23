#pragma once
#include "system.h"
#include "context.h"
#include "camera_component.h"
#include "graphics_component.h"
#include "renderable_mesh_component.h"

class GraphicsSystem :
    public System
{
    bgfx::Context _context;

public:

    GraphicsSystem():
        _context(1000,800)
    {
        _type_name = "graphics";
    }

    virtual void init_update() override 
    {
        auto& graphics_comp = get_array<CompGraphics>()[0];
        graphics_comp.window = _context.get_window();
    }

    void update(double dt) 
    {
        auto& camera = get_array<CompCamera>()[0];
        _context.start_frame();
        auto& meshes = get_array<CompRenderableMesh>();

        for (auto& rmesh : meshes)
        {
            camera.camera.draw_object(rmesh.mesh);
        }
        _context.end_frame();
    }
};

