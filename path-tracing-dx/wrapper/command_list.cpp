#include "command_list.hpp"

ID3D12CommandAllocator* const* path_tracing::dx::wrapper::command_allocator::get_address_of() const
{
	return mAllocator.GetAddressOf();
}

ID3D12CommandAllocator* path_tracing::dx::wrapper::command_allocator::operator->() const
{
	return mAllocator.Get();
}

ID3D12CommandAllocator* path_tracing::dx::wrapper::command_allocator::get() const
{
	return mAllocator.Get();
}

path_tracing::dx::wrapper::command_allocator path_tracing::dx::wrapper::command_allocator::create(const device& device)
{
	command_allocator allocator;

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.mAllocator.GetAddressOf()));

	return allocator;
}

ID3D12GraphicsCommandList4* const* path_tracing::dx::wrapper::graphics_command_list::get_address_of() const
{
	return mCommandList.GetAddressOf();
}

ID3D12GraphicsCommandList4* path_tracing::dx::wrapper::graphics_command_list::operator->() const
{
	return mCommandList.Get();
}

ID3D12GraphicsCommandList4* path_tracing::dx::wrapper::graphics_command_list::get() const
{
	return mCommandList.Get();
}

path_tracing::dx::wrapper::graphics_command_list path_tracing::dx::wrapper::graphics_command_list::create(
	const device& device, const command_allocator& allocator)
{
	graphics_command_list command_list;

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.get(), nullptr,
		IID_PPV_ARGS(command_list.mCommandList.GetAddressOf()));

	command_list->Close();

	return command_list;
}

ID3D12CommandQueue* const* path_tracing::dx::wrapper::command_queue::get_address_of() const
{
	return mCommandQueue.GetAddressOf();
}

ID3D12CommandQueue* path_tracing::dx::wrapper::command_queue::operator->() const
{
	return mCommandQueue.Get();
}

ID3D12CommandQueue* path_tracing::dx::wrapper::command_queue::get() const
{
	return mCommandQueue.Get();
}

void path_tracing::dx::wrapper::command_queue::execute(const std::vector<graphics_command_list>& command_lists) const
{
	std::vector<ID3D12CommandList*> lists;

	for (auto& list : command_lists) lists.push_back(list.get());

	mCommandQueue->ExecuteCommandLists(static_cast<UINT>(lists.size()), lists.data());
}

void path_tracing::dx::wrapper::command_queue::wait()
{
	mCommandQueue->Signal(mFence.Get(), ++mFenceValue);

	if (mFence->GetCompletedValue() < mFenceValue) {
		const auto event_handle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

		mFence->SetEventOnCompletion(mFenceValue, event_handle);

		WaitForSingleObject(event_handle, INFINITE);
		CloseHandle(event_handle);
	}
}

path_tracing::dx::wrapper::command_queue path_tracing::dx::wrapper::command_queue::create(const device& device)
{
	command_queue queue;

	queue.mFenceValue = 0;

	D3D12_COMMAND_QUEUE_DESC desc;

	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	device->CreateCommandQueue(&desc, IID_PPV_ARGS(queue.mCommandQueue.GetAddressOf()));

	device->CreateFence(queue.mFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(queue.mFence.GetAddressOf()));

	return queue;
}
