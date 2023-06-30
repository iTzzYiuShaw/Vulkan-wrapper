#pragma once
#include "buffer.h"
#include "../texture/texture.h"

namespace IP::Wrapper {

    /**
    * Within a descriptorSetLayout, we need to know which uniform exists, the size and the type of it.
    */

	struct UniformParameter {
		using Ptr = std::shared_ptr<UniformParameter>;
		static Ptr create() { return std::make_shared< UniformParameter>(); }

		size_t					mSize{ 0 };
		uint32_t				mBinding{ 0 };


        //For each binding point, there could be more than one uniform, it could be a matrix[]
        //Count represents the size of the array

		uint32_t				mCount{ 0 };
		VkDescriptorType		mDescriptorType;
		VkShaderStageFlagBits	mStage;

		std::vector<Buffer::Ptr> mBuffers{};
		Texture::Ptr mTexture{ nullptr };
	};

}
