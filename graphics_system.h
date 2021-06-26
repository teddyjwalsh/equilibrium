#pragma once

#include <random>

#include "system.h"
#include "context.h"
#include "camera_component.h"
#include "graphics_component.h"
#include "compute_shader.h"
#include "renderable_mesh_component.h"
#include "ray_camera_component.h"
#include "quadtree.h"
#include "noise_gen.h"

extern std::string light_compute_shader;


class GraphicsSystem :
    public System
{
    bgfx::Context _context;
    bgfx::RenderableMesh _quad;
    bgfx::Camera _camera;
    std::shared_ptr<bgfx::Material> _mat;
    bgfx::ComputeShader _cs;
    quadtree::QuadTree _quadtree;
    CombinedNoise _noise;
    int _height_map_x;
    int _height_map_y;
    std::vector<float> _height_map_vector;

    std::default_random_engine _generator;
    std::normal_distribution<double> _distribution;

public:

    GraphicsSystem():
        _context(1920,1080),
        _camera(1,0.8),
        _distribution(5.0, 2.5),
        _quadtree(1024)
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
        //_camera.set_position(glm::vec3(0.5, 0.5, 2));
        //_camera.set_look_target(glm::vec3(0.5, 0.5, 0));
        _camera.set_position(glm::vec3(0.5,0.75,0.5));
        _camera.set_look_target(glm::vec3(0.5,0,0.5));

        auto& graphics_comp = get_array<CompGraphics>()[0];
        graphics_comp.window = _context.get_window();
        int x_res, y_res;
        graphics_comp.get_window_size(x_res, y_res);
        x_res = 500;
        y_res = 300;
        graphics_comp.window = _context.get_window();

        auto quad_mesh = std::make_shared<bgfx::Mesh>();
        auto quad_mat = std::make_shared<bgfx::Material>();

        auto quad_tex = std::make_shared<bgfx::Texture>("quad_tex", x_res, y_res);

        std::vector<unsigned char> test_tex = { {255,0,0,255, 0,255,0,255, 0,0,255,255, 255,255,0,255} };
        std::vector<unsigned char> test_tex2 = { {0,0,0,255, 50,50,50,255, 150,150,150,255, 200,200,200,255} };
        //quad_tex->load_data(test_tex.data(), 2, 2);
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
        
        auto height_map_buffer = std::make_shared<bgfx::Buffer<float>>();


        float block_size = 1;
        int qt_x_size = 2048;
        int qt_y_size = 2048;
        _height_map_x = qt_x_size;
        _height_map_y = qt_y_size;
        _height_map_vector.resize(qt_x_size * qt_y_size);
        
        double max_height;
        for (int i = 0; i < qt_x_size; i += 1)
        {
            for (int j = 0; j < qt_y_size; j += 1)
            {
                double height = _noise.get_point(i, j);
                float dist_from_center = std::sqrt((i - qt_x_size / 2) * (i - qt_x_size / 2) + (j - qt_y_size / 2) * (j - qt_y_size / 2));
                //height += std::pow(std::sqrt(qt_y_size*qt_y_size/4 + qt_y_size*qt_y_size/4) - dist_from_center, 1.7)/2000.0;
                //_quadtree.add_node(glm::vec3(i*block_size, j*block_size, 0), block_size, height);
                _height_map_vector[j * qt_x_size + i] = height;
                max_height = std::max(max_height, height);
            }
        }
        height_map_buffer->set_data(_height_map_vector, bgfx::BindPoint::SHADER_STORAGE_BUFFER);
        _quadtree.load_nodes();

        _cs.add_texture(quad_tex, 1);
        _cs.add_buffer_input(_quadtree.get_buffer(), 2);
        _cs.add_buffer_input(height_map_buffer, 3);
        _cs.set_code(light_compute_shader);
        _cs.set_call_size(x_res, y_res, 1);
        _cs.compile();

        _cs._program.use();
        _cs._program.set_uniform_i1(_cs._program.get_uniform_location("height_map_height"), qt_y_size);
        _cs._program.set_uniform_i1(_cs._program.get_uniform_location("height_map_width"), qt_x_size);
        _cs._program.set_uniform_1f(_cs._program.get_uniform_location("max_height"), max_height);

        _cs.run();

        auto& ray_camera = get_array<CompRayCamera>()[0];
        ray_camera.f = 0.5;
        ray_camera.width = 1.0;
        ray_camera.height = y_res * 1.0 / x_res;
        ray_camera.location = glm::vec3(100, 100, _height_map_vector[100 * qt_x_size + 100] + 100);
        ray_camera.set_look(glm::normalize(glm::vec3(100, 200, 0) - ray_camera.location));
    }

    int height_map_index(int x, int y)
    {
        return std::clamp(int(y), 0, _height_map_y - 1)* _height_map_x + std::clamp(int(x), 0, _height_map_x - 1);
    }

    void update(double dt) 
    {
        // frame initialization
        _context.start_frame();

        // fetch necessary components
        auto& meshes = get_array<CompRenderableMesh>();
        auto& ray_camera = get_array<CompRayCamera>()[0];

        // set shader camera uniforms
        {
            ray_camera.location.z = _height_map_vector[height_map_index(ray_camera.location.x, ray_camera.location.y)] + 100;

            _cs._program.use();
            _cs._program.set_uniform_3f("camera_loc", ray_camera.location);
            _cs._program.set_uniform_3f("camera_look", ray_camera.look);
            _cs._program.set_uniform_3f("camera_right", ray_camera.right);
            _cs._program.set_uniform_3f("camera_up", ray_camera.up);
            _cs._program.set_uniform_1f(_cs._program.get_uniform_location("camera_width"), ray_camera.width);
            _cs._program.set_uniform_1f(_cs._program.get_uniform_location("camera_height"), ray_camera.height);
            _cs._program.set_uniform_1f(_cs._program.get_uniform_location("camera_f"), ray_camera.f);

            _cs.run();
        }

        // draw quad
        {
            _camera.draw_object(_quad);
        }
        _context.end_frame();
    }
};

