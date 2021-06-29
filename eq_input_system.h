#pragma once

#include "input_system.h"
#include "height_map_component.h"

class SysInputEq : public SysInput
{
	void init_update() override
	{
		auto& graphics = get_array<CompGraphics>()[0];
		set_cursor_enabled(true, graphics.window);
	}

	void update(double dt) override
	{
		auto& key_state = get_array<CompKeyState>()[0];
		auto& height_map = get_array<CompHeightMap>()[0];
		auto cam = key_state.sibling<CompRayCamera>();
		auto& graphics = get_array<CompGraphics>()[0];
		int glfw_x_res, glfw_y_res;
		graphics.get_window_size(glfw_x_res, glfw_y_res);

		double prev_mouse_pos_x = key_state.mouse_pos_x;
		double prev_mouse_pos_y = key_state.mouse_pos_y;
		SysInput::update(dt);

		if (key_state.pressed[0])
		{
			double mouse_dx = key_state.mouse_pos_x - prev_mouse_pos_x;
			double mouse_dy = key_state.mouse_pos_y - prev_mouse_pos_y;
			cam->location += glm::vec3(-mouse_dx, mouse_dy,0)*0.1;
			//glm::vec3 temp_look = cam->look;
			//temp_look += -cam->up * mouse_dy * 0.01;
			//temp_look += -cam->right * mouse_dx * 0.01;
			//cam->set_look(glm::normalize(temp_look));
		}
		if (key_state.push[1])
		{
			printf("PUSSHHH\n");
			auto dir = cam->get_ray((key_state.mouse_pos_x * 1.0 / glfw_x_res), (1.0 - key_state.mouse_pos_y * 1.0 / glfw_y_res));
			auto [hit, intersect] = height_map.quadtree.ray_into_height_map_quadtree(cam->location, dir);
			printf("Dir: %f, %f, %f\n", dir.x, dir.y, dir.z);
			printf("Intersect: %f, %f, %f\n", intersect.x, intersect.y, intersect.z);
			printf("Mouse pos: %f, %f\n", cam->location.x, cam->location.y);
 			height_map.color_array[height_map.height_map_index(intersect.x, intersect.y)] = glm::vec4(1,0,0,1);
			height_map.color_changed = true;
		}
		if (key_state.pressed[GLFW_KEY_W])
		{
			cam->location += cam->look;
		}

	}
};