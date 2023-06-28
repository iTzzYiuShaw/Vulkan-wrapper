#pragma once

#include "../base.h"
#include "device.h"
#include "description.h"

namespace IP::Wrapper {

	class DescriptorPool {
	public:
		using Ptr = std::shared_ptr<DescriptorPool>;
		static Ptr create(const Device::Ptr& device) { return std::make_shared<DescriptorPool>(device); }

		DescriptorPool(const Device::Ptr &device);

		~DescriptorPool();

		void build(std::vector<UniformParameter::Ptr>& params, const int &frameCount);

		[[nodiscard]] auto getPool() const { return mPool; }

	private:
		/**
		 * Here, we need to know how many of each type of uniform there are, so that we can reserve space for them.
		 * The 'space' referred to here is not the size of the uniform buffer, but rather the size of the descriptor,
		 * which varies depending on the type of uniform. This size of the descriptor is a concept implicit within the system.
		 */
		VkDescriptorPool mPool{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}