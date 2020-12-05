#pragma once

#include "../render/render_device.hpp"

#include <vector>

namespace path_tracing::runtime::resources {

	using render::render_device;

	struct mesh_info {
		uint32 vtx_location = 0;
		uint32 vtx_count = 0;

		uint32 idx_location = 0;
		uint32 idx_count = 0;

		mesh_info() = default;
	};

	struct mesh_property {
		bool normals = false;
		bool uvs = false;

		mesh_property() = default;
	};
	
	struct mesh_cpu_buffer {
		std::vector<vector3> positions;
		std::vector<vector3> normals;
		std::vector<vector3> uvs;

		std::vector<uint32> indices;

		mesh_cpu_buffer() = default;
	};

	struct mesh_gpu_buffer {
		wrapper::directx12::buffer positions;
		wrapper::directx12::buffer normals;
		wrapper::directx12::buffer uvs;
		
		wrapper::directx12::buffer indices;

		mesh_gpu_buffer() = default;
	};
	
	class meshes_system final : public noncopyable {
	public:
		meshes_system() = default;

		~meshes_system() = default;

		void upload_cached_buffers(const render_device& device);
		
		mesh_info allocate(const std::string& name, const mesh_cpu_buffer& mesh);

		mesh_info allocate(const std::string& name, mesh_cpu_buffer&& mesh);

		mesh_info info(const std::string& name) const;

		mesh_property property(const mesh_info& info) const;
		
		wrapper::directx12::raytracing_geometry geometry(const mesh_info& info) const;
		
		const mesh_cpu_buffer& cpu_buffer() const noexcept;

		const mesh_gpu_buffer& gpu_buffer() const noexcept;
		
		bool has(const std::string& name) const noexcept;

		bool has(const mesh_info& info) const noexcept;

		size_t count() const noexcept;
	private:
		void allocate_gpu_buffer_memory(const render_device& device);

		wrapper::directx12::command_allocator mCommandAllocator;
		wrapper::directx12::graphics_command_list mCommandList;
		
		std::vector<std::pair<std::string, mesh_cpu_buffer>> mCachedMeshes;

		mapping<uint32, wrapper::directx12::raytracing_geometry> mGeometries;
		mapping<uint32, mesh_property> mMeshProperties;
		mapping<std::string, mesh_info> mMeshInfos;

		mesh_cpu_buffer mCpuBuffers;
		mesh_gpu_buffer mGpuBuffers;
		mesh_gpu_buffer mUploads;

		size_t mVtxBaseCount = 0;
		size_t mIdxBaseCount = 0;
	};
	
}
