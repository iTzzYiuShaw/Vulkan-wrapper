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

		//begin  
		//xxx 
		//beginRenderPass 
		//xxxx �󶨸���ʵ������  
		/*while (n < objectNumber) {
			objects[i]->recordCommand()
		}*/
		//endRenderPass 
		//end
		
		//VkCommandBufferUsageFlags
		//VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT��������ֻ�ᱻʹ���ύһ��
		//VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT���������壬��һ���������壬λ��һ��renderPass����
		//VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT:�����Ѿ����ύ�ˣ�ִ���ڼ䣬�����ٴ��ύ

		//VkCommandBufferInheritanceInfo:�����������Ƕ������壬��ô����ṹ�壬��¼������������������Ϣ/�̳���Ϣ
		void begin(VkCommandBufferUsageFlags flag = 0, const VkCommandBufferInheritanceInfo& inheritance = {});

		//VkSubpassContents:
		//VK_SUBPASS_CONTENTS_INLINE:��Ⱦָ��ᱻ��¼������壬�������϶������������
		//VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS:��Ⱦָ������˶���ָ��嵱��,����������������
		//beginRenderPass��ʱ��&&ʹ���˶�������������£�ʹ��
		void beginRenderPass(const VkRenderPassBeginInfo &renderPassBeginInfo, const VkSubpassContents &subPassContents = VK_SUBPASS_CONTENTS_INLINE);

		void bindGraphicPipeline(const VkPipeline &pipeline);

		void draw(size_t vertexCount);

		void endRenderPass();

		void end();

		[[nodiscard]] auto getCommandBuffer() const { return mCommandBuffer; }
	private:
		VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		CommandPool::Ptr mCommandPool{ nullptr };
	};
}