#pragma once

#include "../base.h"
#include "device.h"
#include "description.h"
#include "descriptorSetLayout.h"
#include "descriptorPool.h"

namespace IP::Wrapper {

    /**
     * For each model rendering, a descriptor set needs to be bound, and the binding location is within the command buffer.
     * Each descriptor set corresponds to a buffer used by a VP (View-Projection) matrix, a buffer used by a model matrix, and so on.
     * Within it, there are many descriptions like binding size, etc.
     *
     * Due to the existence of the swap chain,
     * multiple frames may be rendered in parallel, so we need to generate a descriptor set for each image of the swap chain.
     */

	class DescriptorSet {
	public:
		using Ptr = std::shared_ptr<DescriptorSet>;
		static Ptr create(
			const Device::Ptr& device,
			const std::vector<UniformParameter::Ptr> params,
			const DescriptorSetLayout::Ptr& layout,
			const DescriptorPool::Ptr& pool,
			int frameCount
		) { 
			return std::make_shared<DescriptorSet>(
				device,
				params,
				layout,
				pool,
				frameCount
				); 
		}

		DescriptorSet(
			const Device::Ptr &device,
			const std::vector<UniformParameter::Ptr> params,
			const DescriptorSetLayout::Ptr &layout,
			const DescriptorPool::Ptr &pool,
			int frameCount
		);

		~DescriptorSet();

		[[nodiscard]] auto getDescriptorSet(int frameCount) const { return mDescriptorSets[frameCount]; }

	private:
        //Multiple frames may be rendered in parallel
		std::vector<VkDescriptorSet> mDescriptorSets{};
		Device::Ptr mDevice{ nullptr };
	};
}