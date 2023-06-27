#pragma once

#include "../base.h"
#include "commandPool.h"
#include "device.h"

namespace IP::Wrapper {

	class CommandBuffer {
	public:
		using Ptr = std::shared_ptr<CommandBuffer>;
		static Ptr create(const Device::Ptr& device, const CommandPool::Ptr& commandPool, bool asSecondary = false) { 
			return std::make_shared<CommandBuffer>(device, commandPool, asSecondary); 
		}

		CommandBuffer(const Device::Ptr &device, const CommandPool::Ptr &commandPool, bool asSecondary = false);

		~CommandBuffer();

		//VkCommandBufferUsageFlags
		//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT： command will be used once
		//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary buffer, located within the render pass
		//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT: The order has been submitted and it can be submitted again

		//VkCommandBufferInheritanceInfo:"If this command buffer is a secondary buffer,
        // then this structure records its associated primary command information and inheritance information
		void begin(VkCommandBufferUsageFlags flag = 0, const VkCommandBufferInheritanceInfo& inheritance = {});

		//VkSubpassContents:
		//VK_SUBPASS_CONTENTS_INLINE:Rendering commands are recorded in the command buffer, and this command buffer is the primary command buffer.
		//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS:Rendering commands are placed in the secondary command buffer,
        // which is used when the primary command buffer calls beginRenderpass() and uses the secondary buffer

		void beginRenderPass(const VkRenderPassBeginInfo &renderPassBeginInfo, const VkSubpassContents &subPassContents = VK_SUBPASS_CONTENTS_INLINE);

		void bindGraphicPipeline(const VkPipeline &pipeline);

		void draw(size_t vertexCount);

		void endRenderPass();

		void end();

        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, uint32_t copyInfoCount, const std::vector<VkBufferCopy>& copyInfos);

        void submitSync(VkQueue queue, VkFence fence);

		[[nodiscard]] auto getCommandBuffer() const { return mCommandBuffer; }
	private:
		VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		CommandPool::Ptr mCommandPool{ nullptr };
	};
}