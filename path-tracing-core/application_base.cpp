#include "application_base.hpp"
#include "imgui_impl_win32.hpp"

#include <chrono>

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

using time_point = std::chrono::high_resolution_clock;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DefaultWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
		
	switch (message)
	{
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool is_key_down(int keycode) { return KEYDOWN(static_cast<int>(keycode)); }

path_tracing::core::application_base::application_base(const std::string& name, int width, int height) :
	mName(name), mWidth(width), mHeight(height), mHandle(nullptr), mExisted(false)
{
}

path_tracing::core::application_base::~application_base()
{
	mRenderer->release();
	
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void path_tracing::core::application_base::run_loop()
{
	auto current = time_point::now();

	while (mExisted == true) {
		MSG message;

		message.hwnd = static_cast<HWND>(mHandle);

		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);

			process_message(message);
			
			if (message.message == WM_QUIT) mExisted = false;
		}

		if (mExisted == false) break;

		auto duration = std::chrono::duration_cast<
			std::chrono::duration<float>>(time_point::now() - current);

		ImGui_ImplWin32_NewFrame();

		update(duration.count());
		mRenderer->render(mCamera);
		
		current = time_point::now();
	}
}

void path_tracing::core::application_base::update(real delta)
{
	return;
	const real speed = 200;
	
	if (is_key_down(2)) {

		const auto offset = (mMousePosition - mLastMousePosition) * 0.001f;

		quaternion rotation;
		vector3 position;
		vector3 scale;
		vector3 skew;
		vector4 perspective;

		decompose(mCamera->camera_to_world().matrix, scale, rotation,
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

		if (is_key_down('D')) translate = translate - axis_x * delta * speed;
		if (is_key_down('A')) translate = translate + axis_x * delta * speed;
		if (is_key_down('W')) translate = translate - axis_z * delta * speed;
		if (is_key_down('S')) translate = translate + axis_z * delta * speed;

		translate.y = 0;
		
		const auto matrix =
			glm::translate(matrix4x4(1), position + translate) *
			glm::mat4_cast(quaternion(vector3(pitch, yaw, roll))) *
			glm::scale(matrix4x4(1), scale);

		mCamera->set_transform(transform(matrix));
	}
	
	mLastMousePosition = mMousePosition;
}

void path_tracing::core::application_base::initialize_windows()
{
	const auto hInstance = GetModuleHandle(nullptr);
	const auto class_name = mName;

	WNDCLASS appInfo;

	appInfo.style = CS_DBLCLKS;
	appInfo.lpfnWndProc = DefaultWindowProc;
	appInfo.cbClsExtra = 0;
	appInfo.cbWndExtra = 0;
	appInfo.hInstance = hInstance;
	appInfo.hIcon = static_cast<HICON>(LoadImage(nullptr, "", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	appInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
	appInfo.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	appInfo.lpszMenuName = nullptr;
	appInfo.lpszClassName = &class_name[0];

	RegisterClass(&appInfo);

	RECT rect;

	rect.top = 0;
	rect.left = 0;
	rect.right = static_cast<UINT>(mWidth);
	rect.bottom = static_cast<UINT>(mHeight);

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	mHandle = CreateWindow(&class_name[0], &class_name[0], WS_OVERLAPPEDWINDOW ^
		WS_SIZEBOX ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	mExisted = true;

	ShowWindow(static_cast<HWND>(mHandle), SW_SHOW);

	ImGui::CreateContext();
	ImGui_ImplWin32_Init(mHandle);
}

void path_tracing::core::application_base::process_message(const MSG& message)
{
	switch (message.message) {
	case WM_MOUSEMOVE:
	{
		mMousePosition.x = static_cast<float>(LOWORD(message.lParam));
		mMousePosition.y = static_cast<float>(HIWORD(message.lParam));
		break;
	}
	default:
		break;
	}
}

void path_tracing::core::application_base::initialize()
{
	initialize_windows();
	initialize_graphics();
}

void path_tracing::core::application_base::load(const std::shared_ptr<camera>& camera, const std::shared_ptr<scene>& scene,
	const render_config& config)
{
	mCamera = camera;
	mScene = scene;

	mRenderer->build(scene, config);
}
