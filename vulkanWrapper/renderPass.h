#pragma once

#include "../base.h"
#include "device.h"

namespace IP::Wrapper {
	/**
	 *1 attachment
	 * VkAttachmentDescription: Describing a structure for a color or depth stencil attachment, it's not a real attachment, but a mere description.
	 * VkAttachmentDescription1 VkAttachmentDescription2 VkAttachmentDescription3 VkAttachmentDescription4: Announce what formats are required to the external processes
	 * VkAttachmentReference: Illustrate which attachment among all attachments is required, including its index and expected format
	 * VkSubpassDescription:
	 * VkSubpassDependency: Describe dependencies between different subpasses
	*/
	class SubPass {
	public:
		SubPass();

		~SubPass();

		void addColorAttachmentReference(const VkAttachmentReference& ref);

		void addInputAttachmentReference(const VkAttachmentReference& ref);

		void setDepthStencilAttachmentReference(const VkAttachmentReference& ref);

		void buildSubPassDescription();

		[[nodiscard]] auto getSubPassDescription() const { return mSubPassDescription; }

	private:
		VkSubpassDescription mSubPassDescription{};
		std::vector<VkAttachmentReference> mColorAttachmentReferences{};
		std::vector<VkAttachmentReference> mInputAttachmentReferences{};
		VkAttachmentReference mDepthStencilAttachmentReference{};
	};



    //Render pass
	class RenderPass {
	public:
		using Ptr = std::shared_ptr<RenderPass>;

		static Ptr create(const Device::Ptr& device) { return std::make_shared<RenderPass>(device); }
		
		RenderPass(const Device::Ptr &device);

		~RenderPass();

		void addSubPass(const SubPass& subpass);

		void addDependency(const VkSubpassDependency &dependency);

		void addAttachment(const VkAttachmentDescription &attachmentDes);

		void buildRenderPass();

		[[nodiscard]] auto getRenderPass() const { return mRenderPass; }

	private:
		VkRenderPass mRenderPass{VK_NULL_HANDLE};

		std::vector<SubPass> mSubPasses{};
		std::vector<VkSubpassDependency> mDependencies{};
		std::vector<VkAttachmentDescription> mAttachmentDescriptions{};

		Device::Ptr mDevice{ nullptr };
	};
}