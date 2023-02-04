#include "view_window.hpp"

#include "../../extensions/imgui/imgui_impl_win32.hpp"
#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

#include <Windows.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT DefaultWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

raytracing::runtime::windows::view_window::view_window(
	const wrapper::directx12::command_queue& queue,
	const wrapper::directx12::device& device, 
	const std::string& name, uint32 size_x, uint32 size_y) :
	mSizeX(size_x), mSizeY(size_y)
{
	const auto hInstance = GetModuleHandle(nullptr);
	const auto class_name = name;

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
	rect.right = static_cast<UINT>(size_x);
	rect.bottom = static_cast<UINT>(size_y);

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	mHandle = CreateWindow(&class_name[0], &class_name[0], WS_OVERLAPPEDWINDOW ^
		WS_SIZEBOX ^ WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	mLiving = true;

	ShowWindow(static_cast<HWND>(mHandle), SW_SHOW);

	ImGui::CreateContext();
	
	ImGui_ImplWin32_Init(mHandle);

	mSwapChain = wrapper::directx12::swap_chain::create(queue,
		static_cast<int>(size_x), static_cast<int>(size_y),
		static_cast<HWND>(mHandle));
}

raytracing::runtime::windows::view_window::view_window(view_window&& window) noexcept :
	mSwapChain(window.mSwapChain), mSizeX(window.mSizeX), mSizeY(window.mSizeY),
	mHandle(window.mHandle), mLiving(window.mLiving)
{
	window.mHandle = nullptr;
	window.mLiving = false;
}

raytracing::runtime::windows::view_window::~view_window()
{
	if (mHandle != nullptr) {
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void raytracing::runtime::windows::view_window::update(const runtime_service& service, const runtime_frame& frame)
{
	MSG message;

	message.hwnd = static_cast<HWND>(mHandle);

	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT) mLiving = false;
	}

	ImGui_ImplWin32_NewFrame();
}

void raytracing::runtime::windows::view_window::present(bool sync) const
{
	mSwapChain.present(sync);
}

bool raytracing::runtime::windows::view_window::living() const noexcept
{
	return mLiving;
}

raytracing::uint32 raytracing::runtime::windows::view_window::size_x() const noexcept
{
	return mSizeX;
}

raytracing::uint32 raytracing::runtime::windows::view_window::size_y() const noexcept
{
	return mSizeY;
}

raytracing::runtime::windows::view_window& raytracing::runtime::windows::view_window::operator=(
	view_window&& window) noexcept
{
	mSwapChain = window.mSwapChain;
	mSizeX = window.mSizeX;
	mSizeY = window.mSizeY;
	mHandle = window.mHandle;
	mLiving = window.mLiving;
	
	window.mHandle = nullptr;
	window.mLiving = false;

	return *this;
}
