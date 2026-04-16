#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "orbit/core/system.h"
#include "orbit/core/input.h"

namespace orbit::graphics::camera
{
	namespace
	{
		input::cursor_position _last_position;
		camera_buffer _buffer;

		glm::vec3 _position = glm::vec3(0.f, 0.f, 5.f);
		glm::vec3 _up = glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 _forward = glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 _right = glm::vec3(1.f, 0.f, 0.f);

		float total_roll = 0.f;
	}

	void initialize()
	{
		_buffer.view = glm::lookAt(_position, _position + _forward, _up);
		_buffer.view = glm::transpose(_buffer.view);

		_buffer.projection = glm::perspective(glm::radians(45.f), float(system::get_window_width()) / float(system::get_window_height()), 0.1f, 100.f);
		_buffer.projection = glm::transpose(_buffer.projection);

		_last_position = input::get_cursor_position();
	}

	void update()
	{
		float roll = 0.f, yaw = 0.f;

		auto cursor_pos = input::get_cursor_position();

		float cursor_multiplier = 0.001;

		roll = (_last_position._ypos - cursor_pos._ypos) * cursor_multiplier;
		yaw = (_last_position._xpos - cursor_pos._xpos) * cursor_multiplier;

		total_roll += roll;
		float old_total_roll = total_roll;
		total_roll = glm::clamp(total_roll, -glm::pi<float>()/2, glm::pi<float>()/2);
		if ( old_total_roll != total_roll)
			roll = 0.f;

		glm::mat4 camera_rotation = glm::rotate(glm::mat4(1.f), roll, _right);
		camera_rotation = glm::rotate(camera_rotation, yaw, _up);

		_forward = glm::vec3(camera_rotation * glm::vec4(_forward, 0.f));
		_up = glm::vec3(camera_rotation * glm::vec4(_up, -0.f));
		_right = glm::cross(_forward, _up);

		if ( input::get_key(GLFW_KEY_W))
			_position += _forward * 0.02f;
		if ( input::get_key(GLFW_KEY_S))
			_position += _forward * -0.02f;
		if ( input::get_key(GLFW_KEY_A))
			_position += _right * -0.02f;
		if ( input::get_key(GLFW_KEY_D))
			_position += _right * 0.02f;

		if ( input::get_key(GLFW_KEY_Z))
			_position += _up * 0.02f;
		if ( input::get_key(GLFW_KEY_X))
			_position += _up * -0.02f;

		initialize();
	}

	const camera_buffer& get_camera_buffer()
	{
		return _buffer;
	}
}