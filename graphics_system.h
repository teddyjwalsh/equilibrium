#pragma once
#include "system.h"
#include "context.h"
#include "camera_component.h"
#include "graphics_component.h"
#include "compute_shader.h"
#include "renderable_mesh_component.h"

class GraphicsSystem :
    public System
{
    bgfx::Context _context;
    bgfx::RenderableMesh _quad;
    bgfx::Camera _camera;
    std::shared_ptr<bgfx::Material> _mat;

public:

    GraphicsSystem():
        _context(1000,800),
        _camera(1,0.8)
    {
        _type_name = "graphics";
    }

    virtual void init_update() override 
    {
        //_camera.set_position(glm::vec3(0.5, 0.5, 2));
        //_camera.set_look_target(glm::vec3(0.5, 0.5, 0));
        _camera.set_position(glm::vec3(0.5,0.75,0.5));
        _camera.set_look_target(glm::vec3(0.5,0,0.5));

        auto& graphics_comp = get_array<CompGraphics>()[0];
        graphics_comp.window = _context.get_window();
        int x_res, y_res;
        graphics_comp.get_window_size(x_res, y_res);
        graphics_comp.window = _context.get_window();

        auto quad_mesh = std::make_shared<bgfx::Mesh>();
        auto quad_mat = std::make_shared<bgfx::Material>();

        auto quad_tex = std::make_shared<bgfx::Texture>("quad_tex", x_res, y_res);

        std::vector<unsigned char> test_tex = { {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255} };
        std::vector<unsigned char> test_tex2 = { {0,0,0,255, 50,50,50,255, 150,150,150,255, 200,200,200,255} };
        quad_tex->load_data(test_tex.data(), 2, 2);
        quad_tex->set_interpolation_mode(bgfx::Texture::InterpMode::NEAREST);

        bgfx::Material::MatNode* tex_out;
        tex_out = quad_mat->add_texture(quad_tex);
        //tex_out2 = quad_mat->add_texture(quad_tex2);
        auto sum_node = quad_mat->sum_node("vec4");
        auto uv_node = quad_mat->uv_node();
        //tex_out->connect(&sum_node->inputs["in1"]);
        //tex_out2->connect(&sum_node->inputs["in2"]);
        tex_out->connect(&quad_mat->frag_color_node()->inputs["gl_FragColor"]);
        uv_node->connect("uv", &tex_out->inputs["tex_coord"]);
        //uv_node->connect("uv", &tex_out2->inputs["tex_coord"]);
        //sum_node->connect(&quad_mat->frag_color_node()->inputs["gl_FragColor"]);

        quad_mat->compile();

        //quad_mesh->set_vertices({ {1,0,0,0,1,0,0,0,1,1,1,0} });
        quad_mesh->set_vertices({ {1,0,0, 0,0,1, 0,0,0, 0,0,1, 1,0,1, 1,0,0} });
        quad_mesh->set_uv_coords({ {1,0, 0,1, 0,0, 0,1, 1,1, 1,0} });

        _quad.set_mesh(quad_mesh);
        _quad.set_material(quad_mat);
        _mat = quad_mat;


        bgfx::ComputeShader cs;
        cs.add_texture(quad_tex, 1);
        cs.set_code(bgfx::cs_string);
        cs.set_call_size(2, 2, 1);
        cs.compile();

        cs.run();
    }

    void update(double dt) 
    {
        //auto& camera = get_array<CompCamera>()[0];
        _context.start_frame();
        auto& meshes = get_array<CompRenderableMesh>();

        //for (auto& rmesh : meshes)
        {
            _camera.draw_object(_quad);
        }
        _context.end_frame();
    }
};

