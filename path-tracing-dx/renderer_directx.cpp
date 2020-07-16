#include "renderer_directx.hpp"

#include "utilities/imgui_impl_dx12.hpp"

path_tracing::dx::renderer_directx::renderer_directx(void* handle, int width, int height) :
	renderer_backend(handle, width, height)
{
	mDevice = std::make_shared<device>(D3D_FEATURE_LEVEL_12_1);

	mCommandAllocator = std::make_shared<command_allocator>(mDevice);
	mCommandQueue = std::make_shared<command_queue>(mDevice);

	mImGuiCommandList = std::make_shared<graphics_command_list>(mCommandAllocator, mDevice);
	mSceneCommandList = std::make_shared<graphics_command_list>(mCommandAllocator, mDevice);

	mSwapChain = std::make_shared<swap_chain>(mCommandQueue, mDevice, mWidth, mHeight, mHandle);

	mRenderTarget = std::make_shared<texture2d>(mDevice, D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_HEAP_TYPE_DEFAULT,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		static_cast<size_t>(width * 0.75),
		static_cast<size_t>(height * 0.75));

	mRenderTargetViewHeap = std::make_shared<descriptor_heap>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, mSwapChain->count());
	mImGuiDescriptorHeap = std::make_shared<descriptor_heap>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 16);

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};

	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Format = mRenderTarget->format();
	srv_desc.Texture2D.MipLevels = 1;

	(*mDevice)->CreateShaderResourceView(mRenderTarget->get(), &srv_desc, mImGuiDescriptorHeap->cpu_handle(1));

	ImGui_ImplDX12_Init(mDevice->get(), static_cast<int>(mSwapChain->count()),
		mSwapChain->format(), mImGuiDescriptorHeap->get(),
		mImGuiDescriptorHeap->cpu_handle(),
		mImGuiDescriptorHeap->gpu_handle());

	for (size_t index = 0; index < mSwapChain->count(); index++) {
		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};

		rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Format = mSwapChain->format();
		rtv_desc.Texture2D.PlaneSlice = 0;
		rtv_desc.Texture2D.MipSlice = 0;

		(*mDevice)->CreateRenderTargetView(mSwapChain->buffer(index)->get(), &rtv_desc, mRenderTargetViewHeap->cpu_handle(index));
	}
}

path_tracing::dx::renderer_directx::~renderer_directx()
{
}

void path_tracing::dx::renderer_directx::render(const std::shared_ptr<core::camera>& camera)
{
	(*mCommandAllocator)->Reset();

	render_scene();
	render_imgui();

	mCommandQueue->execute({ mSceneCommandList, mImGuiCommandList });
	
	mSwapChain->present();

	mCommandQueue->wait();
}

void path_tracing::dx::renderer_directx::build(const std::shared_ptr<core::scene>& scene)
{
	mResourceCache = std::make_shared<resource_cache>(mDevice);
	mResourceScene = std::make_shared<resource_scene>(mDevice);
	mScenePipeline = std::make_shared<scene_pipeline>(mDevice);

	for (const auto& entity : scene->entities()) mResourceCache->cache_entity(entity);

	mResourceCache->execute(mCommandQueue);
	
	mResourceScene->set_entities(scene->entities(), mResourceCache);
	mResourceScene->set_render_target(mRenderTarget);

	mResourceScene->execute(mCommandQueue);
	
	mScenePipeline->set_resource_cache(mResourceCache);
	mScenePipeline->set_resource_scene(mResourceScene);
	mScenePipeline->set_entities(scene->entities());
	mScenePipeline->set_max_depth(5);

	mScenePipeline->execute(mCommandQueue);
}

void path_tracing::dx::renderer_directx::resize(int new_width, int new_height)
{
	mSwapChain->resize(mDevice, new_width, new_height);

	for (size_t index = 0; index < mSwapChain->count(); index++) {
		D3D12_RENDER_TARGET_VIEW_DESC desc;

		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Format = mSwapChain->format();
		desc.Texture2D.PlaneSlice = 0;
		desc.Texture2D.MipSlice = 0;

		(*mDevice)->CreateRenderTargetView(mSwapChain->buffer(index)->get(), &desc, mRenderTargetViewHeap->cpu_handle(index));
	}
}

void path_tracing::dx::renderer_directx::release()
{
	ImGui_ImplDX12_Shutdown();
}

void path_tracing::dx::renderer_directx::render_scene() const
{
	(*mSceneCommandList)->Reset(mCommandAllocator->get(), nullptr);

	mResourceScene->render(mSceneCommandList);
	mScenePipeline->render(mSceneCommandList);

	(*mSceneCommandList)->Close();
}

void path_tracing::dx::renderer_directx::render_imgui() const
{
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Window");
	
	ImGui::Image(reinterpret_cast<ImTextureID>(mImGuiDescriptorHeap->gpu_handle(1).ptr), ImVec2(
		static_cast<float>(mRenderTarget->width()), 
		static_cast<float>(mRenderTarget->height())));

	ImGui::End();
	
	const auto current_frame_index = mSwapChain->current_frame_index();

	const auto before_barrier = mSwapChain->buffer(current_frame_index)->barrier(
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	const auto after_barrier = mSwapChain->buffer(current_frame_index)->barrier(
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	const float clear_color[4] = { 1, 1, 1, 1 };

	const auto render_target_view = mRenderTargetViewHeap->cpu_handle(current_frame_index);
	
	const auto command_list = mImGuiCommandList->get();
	
	command_list->Reset(mCommandAllocator->get(), nullptr);
	command_list->ResourceBarrier(1, &before_barrier);
	command_list->ClearRenderTargetView(render_target_view, clear_color, 0, nullptr);
	command_list->OMSetRenderTargets(1, &render_target_view, false, nullptr);
	command_list->SetDescriptorHeaps(1, mImGuiDescriptorHeap->get_address_of());

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);

	command_list->ResourceBarrier(1, &after_barrier);
	command_list->Close();
}
