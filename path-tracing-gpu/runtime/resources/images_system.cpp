#include "images_system.hpp"

namespace path_tracing::runtime::resources {

	template <typename T>
	void copy_memory(
		const wrapper::directx12::graphics_command_list& command_list,
		const wrapper::directx12::texture2d& destination, 
		const wrapper::directx12::buffer& upload, 
		const std::vector<T>& values)
	{
		const auto logic_width = destination.size_x() *
			wrapper::directx12::size_of(destination.format());

		if (destination.alignment() == logic_width)
			upload.copy_from_cpu(values.data(), sizeof(T) * values.size());
		else {
			const auto upload_memory = static_cast<byte*>(upload.begin_mapping());
			const auto cpu_memory = reinterpret_cast<const byte*>(values.data());
			const auto alignment = destination.alignment();

			for (size_t y = 0; y < destination.size_y(); y++) {
				std::memcpy(upload_memory + y * alignment, cpu_memory + y * logic_width,
					logic_width);
			}

			upload.end_mapping();
		}

		destination.copy_from(command_list, upload);
		destination.barrier(command_list, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	}
	
}

void path_tracing::runtime::resources::images_system::upload_cached_buffers(const render_device& device)
{
	if (mCachedTextures.empty()) return;

	if (mCommandAllocator.get() == nullptr) {
		mCommandAllocator = wrapper::directx12::command_allocator::create(device.device());
		mCommandList = wrapper::directx12::graphics_command_list::create(device.device(), mCommandAllocator);
	}

	mCpuBuffers.clear();
	
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.get(), nullptr);

	for (const auto& cache_texture : mCachedTextures) {
		const auto texture = wrapper::directx12::texture2d::create(
			device.device(),
			wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_COPY_DEST),
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			cache_texture.second.size_x,
			cache_texture.second.size_y
		);

		const auto buffer = wrapper::directx12::buffer::create(
			device.device(),
			wrapper::directx12::resource_info::upload(),
			static_cast<size_t>(cache_texture.second.size_y) * texture.alignment()
		);

		copy_memory(mCommandList, texture, buffer, cache_texture.second.values);
		
		mTextures[mIdentities[cache_texture.first]] = texture;
		
		mCpuBuffers[cache_texture.first] = buffer;
	}

	mCachedTextures.clear();
	
	mCommandList.close();

	device.queue().execute({ mCommandList });
}

path_tracing::identity path_tracing::runtime::resources::images_system::allocate(const std::string& name,
	const image_info<real>& info)
{
	mIdentities.insert({ name, static_cast<identity>(mIdentities.size()) });

	// allocate texture 2d objects memory
	mTextures.push_back(wrapper::directx12::texture2d());
	
	mCachedTextures.insert({ name, info });

	return mIdentities.at(name);
}

path_tracing::identity path_tracing::runtime::resources::images_system::allocate(const std::string& name,
	image_info<real>&& info)
{
	mIdentities.insert({ name, static_cast<identity>(mIdentities.size()) });

	// allocate texture 2d objects memory
	mTextures.push_back(wrapper::directx12::texture2d());

	mCachedTextures.insert({ name, info });

	return mIdentities.at(name);
}

path_tracing::identity path_tracing::runtime::resources::images_system::info(const std::string& name) const
{
	return mIdentities.at(name);
}

wrapper::directx12::texture2d path_tracing::runtime::resources::images_system::texture(identity identity) const
{
	return mTextures[identity];
}

bool path_tracing::runtime::resources::images_system::has(const std::string& name) const noexcept
{
	return mIdentities.find(name) != mIdentities.end();
}

bool path_tracing::runtime::resources::images_system::has(identity identity) const noexcept
{
	return identity < mTextures.size();
}

size_t path_tracing::runtime::resources::images_system::count() const noexcept
{
	return mTextures.size();
}
