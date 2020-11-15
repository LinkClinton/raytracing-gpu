#include "imgui_renderer.hpp"

#include "../../runtime/runtime_service.hpp"

void path_tracing::extensions::imgui::imgui_renderer::resolve(
	const wrapper::directx12::swap_chain& swap_chain,
	const render_device& device, uint32 font_size)
{
	mDevice = device.device();
	mSwapChain = swap_chain;
	
	mCommandAllocator = wrapper::directx12::command_allocator::create(mDevice);
	mCommandList = wrapper::directx12::graphics_command_list::create(mDevice, mCommandAllocator);

	mImGuiDescriptorHeap = wrapper::directx12::descriptor_heap::create(mDevice,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2);

	mRenderTargetViewHeap = wrapper::directx12::descriptor_heap::create(mDevice,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV, mSwapChain.buffers().size());

	for (size_t index = 0; index < mSwapChain.buffers().size(); index++) {
		device.device().create_render_target_view(
			wrapper::directx12::resource_view::render_target2d(mSwapChain.format()),
			mRenderTargetViewHeap.cpu_handle(index),
			mSwapChain.buffers()[index]
		);
	}

	ImGui::StyleColorsLight();

	ImGui::GetIO().Fonts->AddFontFromFileTTF("./resources/fonts/CascadiaCode.ttf",
		static_cast<float>(font_size));

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	wrapper::directx12::extensions::imgui_context::initialize(
		mDevice, mImGuiDescriptorHeap,
		mImGuiDescriptorHeap.cpu_handle(), mImGuiDescriptorHeap.gpu_handle(),
		swap_chain.format(), mSwapChain.buffers().size());

	wrapper::directx12::extensions::imgui_context::set_multi_sample(1);
}

void path_tracing::extensions::imgui::imgui_renderer::update(const runtime_service& service, real delta)
{
	wrapper::directx12::extensions::imgui_context::new_frame();
	ImGui::NewFrame();
}

void path_tracing::extensions::imgui::imgui_renderer::render(const runtime_service& service, real delta)
{
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.get(), nullptr);

	const auto frame_index = mSwapChain->GetCurrentBackBufferIndex();
	const auto render_target_view = mRenderTargetViewHeap.cpu_handle(frame_index);
	
	mSwapChain.buffers()[frame_index].barrier(mCommandList, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	mCommandList.clear_render_target_view(render_target_view, { 1,1,1,1 });

	mCommandList.set_render_targets({ render_target_view });
	mCommandList.set_descriptor_heaps({ mImGuiDescriptorHeap.get() });

	ImGui::Render();

	wrapper::directx12::extensions::imgui_context::render(mCommandList, ImGui::GetDrawData());

	mSwapChain.buffers()[frame_index].barrier(mCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	mCommandList->Close();

	service.render_device.queue().execute({ mCommandList });
}

ImTextureID path_tracing::extensions::imgui::imgui_renderer::allocate(const wrapper::directx12::texture2d& resource,
	size_t index) const
{
	mDevice.create_shader_resource_view(
		wrapper::directx12::resource_view::texture2d(resource.format()),
		mImGuiDescriptorHeap.cpu_handle(index), resource);

	return reinterpret_cast<ImTextureID>(mImGuiDescriptorHeap.gpu_handle(index).ptr);
}
