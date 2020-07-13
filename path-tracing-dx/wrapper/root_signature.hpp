#pragma once

#include "descriptor_heap.hpp"

namespace path_tracing::dx::wrapper {

	class root_signature final {
	public:
		root_signature() = default;

		~root_signature() = default;

		ID3D12RootSignature* const* get_address_of() const;
		ID3D12RootSignature* operator->() const;
		ID3D12RootSignature* get() const;

		void add_descriptor(const std::string& name, const D3D12_ROOT_PARAMETER_TYPE& type, size_t base, size_t space);

		void add_descriptor_table(const std::string& name, const descriptor_table& table);

		void add_constants(const std::string& name, size_t base, size_t space, size_t count);

		void add_srv(const std::string& name, size_t base, size_t space);

		void add_uav(const std::string& name, size_t base, size_t space);

		void add_cbv(const std::string& name, size_t base, size_t space);

		void serialize(const device& device);

		size_t offset(const std::string& name) const;

		size_t size(const std::string& name) const;

		static root_signature create();
	private:
		void push_root_parameter(const std::string& name, const D3D12_ROOT_PARAMETER& parameter);

		ComPtr<ID3D12RootSignature> mRootSignature;

		std::unordered_map<std::string, size_t> mDescriptorIndex;

		std::vector<D3D12_ROOT_PARAMETER> mRootParameters;
		std::vector<size_t> mDescriptorOffset;
		std::vector<size_t> mDescriptorSize;
	};
	
}
