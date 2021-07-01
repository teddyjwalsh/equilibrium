#pragma once

#include "system.h"
#include "context.h"
#include "renderable_mesh_component.h"
#include "graphics_component.h"
#include "camera_component.h"
#include "noise_gen.h"

class SysGLGraphics : public System
{
    bgfx::Context _context;
    bgfx::RenderableMesh _quad;
    CombinedNoise _noise;

public:

    SysGLGraphics() :
        _context(1920, 1080)
    {
        _type_name = "graphics";
        //_noise.add_noise(15, 1.0);
        _noise.add_noise(5, 2.0);
        _noise.add_noise(2, 5.0);
        _noise.add_noise(0.5, 20.0);
        _noise.add_noise(0.01, 100.0);
    }

    virtual void init_update() override
    {
        auto& graphics_comp = get_array<CompGraphics>()[0];
        graphics_comp.window = _context.get_window();
        int glfw_x_res, glfw_y_res;
        graphics_comp.get_window_size(glfw_x_res, glfw_y_res);

        auto& ray_camera = get_array<CompCamera>()[0];
        ray_camera.camera.set_position(glm::vec3(-50, -50, 150));
        ray_camera.camera.set_look_target((glm::vec3(1000, 1000, 0)));
    }

    void update(double dt)
    {
        
        // frame initialization
        _context.start_frame();

        // fetch necessary components
        auto& ray_camera = get_array<CompCamera>()[0];
        auto& renderable_array = get_array<CompRenderableMesh>();

        for (auto& rm : renderable_array)
        {
            auto pos_comp = rm.sibling<CompPosition>();
            if (pos_comp)
            {
                rm.mesh.set_position(pos_comp->pos);
            }
            ray_camera.camera.draw_object(rm.mesh);
        }
        _context.end_frame();
    }

};