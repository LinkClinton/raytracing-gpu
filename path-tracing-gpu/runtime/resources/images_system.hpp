#pragma once

#include "../../extensions/images/image_info.hpp"
#include "../render/render_device.hpp"

namespace path_tracing::runtime::resources {

	using extensions::images::image_info;
	using render::render_device;
	
	class images_system final {
	public:
		images_system() = default;

		~images_system() = default;

		void upload_cached_buffers(const render_device& device);

		identity allocate(const std::string& name, const image_info<real>& info);

		identity allocate(const std::string& name, image_info<real>&& info);
		
		identity info(const std::string& name) const;

		wrapper::directx12::texture2d texture(identity identity) const;
		
		bool has(const std::string& name) const noexcept;

		bool has(identity identity) const noexcept;

		size_t count() const noexcept;
	private:
		std::vector<wrapper::directx12::texture2d> mTextures;

		wrapper::directx12::command_allocator mCommandAllocator;
		wrapper::directx12::graphics_command_list mCommandList;
		
		mapping<std::string, wrapper::directx12::buffer> mCpuBuffers;
		mapping<std::string, image_info<real>> mCachedTextures;
		
		mapping<std::string, identity> mIdentities;
	};

}
