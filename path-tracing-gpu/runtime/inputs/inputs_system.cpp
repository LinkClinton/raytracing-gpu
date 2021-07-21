#include "inputs_system.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

namespace path_tracing::runtime::inputs {

	bool is_key_down(int keycode) { return KEYDOWN(static_cast<int>(keycode)); }
	
}

void path_tracing::runtime::inputs::inputs_system::resolve(const runtime_service& service)
{
	
}

void path_tracing::runtime::inputs::inputs_system::release(const runtime_service& service)
{
	
}

void path_tracing::runtime::inputs::inputs_system::update(const runtime_service& service,
	const runtime_frame& frame)
{
	if (!service.window_system.living()) return;
	
	const auto mouse_position = vector2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	const real speed = 2;

	if (is_key_down(2)) {

		const auto offset = (mouse_position - mLastMousePosition) * 0.001f;

		quaternion rotation;
		vector3 position;
		vector3 scale;
		vector3 skew;
		vector4 perspective;

		decompose(service.scene.camera.transform.matrix(), scale, rotation,
			position, skew, perspective);

		auto yaw = glm::yaw(rotation) + offset.x;
		auto pitch = glm::pitch(rotation) + offset.y;
		auto roll = glm::roll(rotation);

		if (pitch > glm::radians(89.8f)) pitch = glm::radians(89.8f);
		if (pitch < glm::radians(-89.8f)) pitch = glm::radians(-89.8f);

		const auto rotation_matrix = glm::mat4_cast(quaternion(vector3(pitch, yaw, roll)));

		const auto axis_x = glm::normalize(vector3(rotation_matrix * vector4(1, 0, 0, 0)));
		const auto axis_z = glm::normalize(vector3(rotation_matrix * vector4(0, 0, 1, 0)));

		auto translate = vector3();

		if (is_key_down('D')) translate = translate - axis_x * frame.delta_time * speed;
		if (is_key_down('A')) translate = translate + axis_x * frame.delta_time * speed;
		if (is_key_down('W')) translate = translate - axis_z * frame.delta_time * speed;
		if (is_key_down('S')) translate = translate + axis_z * frame.delta_time * speed;

		translate.y = 0;

		const auto matrix =
			glm::translate(matrix4x4(1), position + translate) *
			glm::mat4_cast(quaternion(vector3(pitch, yaw, roll))) *
			glm::scale(matrix4x4(1), scale);

		service.scene.camera.transform = scenes::transform(matrix);
	}

	mLastMousePosition = mouse_position;
	
}
