#pragma once

#include "../../path-tracing-core/shared/transform.hpp"

#include "shader_resource.hpp"
#include "device.hpp"

namespace path_tracing::dx::wrapper {

	using namespace core::shared;
	
	class raytracing_geometry final : public noncopyable {
	public:
		explicit raytracing_geometry(
			const std::shared_ptr<buffer>& positions,
			const std::shared_ptr<buffer>& indices);

		~raytracing_geometry() = default;

		void build(
			const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS& flags,
			const std::shared_ptr<graphics_command_list>& command_list, 
			const std::shared_ptr<device>& device);

		std::shared_ptr<buffer> acceleration() const noexcept;
	private:
		D3D12_RAYTRACING_GEOMETRY_DESC mDesc = {};
		
		std::shared_ptr<buffer> mAcceleration;
		std::shared_ptr<buffer> mScratch;
	};

	class raytracing_instance final : public noncopyable {
	public:
		explicit raytracing_instance(const std::shared_ptr<raytracing_geometry>& geometry, const matrix4x4& transform);

		~raytracing_instance() = default;

		std::shared_ptr<raytracing_geometry> geometry() const noexcept;

		matrix4x4 transform() const noexcept;
	private:
		std::shared_ptr<raytracing_geometry> mGeometry;
		
		matrix4x4 mTransform = matrix4x4(1);
	};

	class raytracing_acceleration final : public noncopyable {
	public:
		explicit raytracing_acceleration(const std::vector<std::shared_ptr<raytracing_instance>>& instances);

		~raytracing_acceleration() = default;

		void build(
			const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS& flags,
			const std::shared_ptr<graphics_command_list>& command_list,
			const std::shared_ptr<device>& device);
	private:
		std::shared_ptr<buffer> mAcceleration;
		std::shared_ptr<buffer> mScratch;

		std::shared_ptr<buffer> mGpuInstance;
		std::shared_ptr<buffer> mCpuInstance;

		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> mInstancesDesc;
	};
	
}
