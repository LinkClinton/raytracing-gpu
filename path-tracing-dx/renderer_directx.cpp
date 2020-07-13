#include "renderer_directx.hpp"

#include "utilities/imgui_impl_dx12.hpp"

path_tracing::dx::renderer_directx::renderer_directx(void* handle, int width, int height) :
	renderer_backend(handle, width, height)
{
	mDevice = device::create();

	mCommandAllocator = command_allocator::create(mDevice);
	mCommandQueue = command_queue::create(mDevice);

	mCommandList = graphics_command_list::create(mDevice, mCommandAllocator);

	mImGuiDescriptorHeap = descriptor_heap::create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 16);
	
	mSwapChain = swap_chain::create(mDevice, mCommandQueue, mWidth, mHeight, mHandle);

	ImGui_ImplDX12_Init(mDevice.get(), static_cast<int>(mSwapChain.count()),
		mSwapChain.format(), mImGuiDescriptorHeap.get(),
		mImGuiDescriptorHeap.cpu_handle(),
		mImGuiDescriptorHeap.gpu_handle());
}

path_tracing::dx::renderer_directx::~renderer_directx()
{
}

void path_tracing::dx::renderer_directx::render(const std::shared_ptr<core::camera>& camera)
{
	// todo:

	render_imgui();
	
	mSwapChain.present();

	mCommandQueue.wait();
}

void path_tracing::dx::renderer_directx::build(const std::shared_ptr<core::scene>& scene)
{
	// todo:
}

void path_tracing::dx::renderer_directx::resize(int new_width, int new_height)
{
	mSwapChain.resize(mDevice, new_width, new_height);
}

void path_tracing::dx::renderer_directx::release()
{
	ImGui_ImplDX12_Shutdown();
}

void path_tracing::dx::renderer_directx::render_imgui() const
{
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();

	const auto current_frame_index = mSwapChain->GetCurrentBackBufferIndex();

	const auto before_barrier = shader_resource::barrier_transition(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET,
		mSwapChain.buffer(current_frame_index));

	const auto after_barrier = shader_resource::barrier_transition(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT,
		mSwapChain.buffer(current_frame_index));

	const float clear_color[4] = { 1, 1, 1, 1 };

	const auto render_target_view = mSwapChain.render_target_view(current_frame_index);
	
	mCommandAllocator->Reset();
	
	mCommandList->Reset(mCommandAllocator.get(), nullptr);
	mCommandList->ResourceBarrier(1, &before_barrier);
	mCommandList->ClearRenderTargetView(render_target_view, clear_color, 0, nullptr);
	mCommandList->OMSetRenderTargets(1, &render_target_view, false, nullptr);
	mCommandList->SetDescriptorHeaps(1, mImGuiDescriptorHeap.get_address_of());

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.get());

	mCommandList->ResourceBarrier(1, &after_barrier);
	mCommandList->Close();

	mCommandQueue.execute({ mCommandList });
}
