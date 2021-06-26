#pragma once

#include "input_system.h"

class SysInputEq : public SysInput
{
	void init_update() override
	{
		auto& key_state = get_array<CompGraphics>()[0];
		set_cursor_enabled(true, key_state.window);
	}

	void update(double dt) override
	{
		auto& key_state = get_array<CompKeyState>()[0];
		auto cam = key_state.sibling<CompRayCamera>();
		double prev_mouse_pos_x = key_state.mouse_pos_x;
		double prev_mouse_pos_y = key_state.mouse_pos_y;
		SysInput::update(dt);

		if (key_state.pressed[0])
		{
			double mouse_dx = key_state.mouse_pos_x - prev_mouse_pos_x;
			double mouse_dy = key_state.mouse_pos_y - prev_mouse_pos_y;
			glm::vec3 temp_look = cam->look;
			temp_look += -cam->up * mouse_dy * 0.01;
			temp_look += -cam->right * mouse_dx * 0.01;
			cam->set_look(glm::normalize(temp_look));
		}
		if (key_state.pressed[GLFW_KEY_W])
		{
			cam->location += cam->look;
		}
	}
};