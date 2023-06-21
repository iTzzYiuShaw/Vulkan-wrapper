#pragma once

#include "../base.h"
#include "device.h"

namespace IP::Wrapper {
	//˼·
	/*
	* 1 attachment  
	VkAttachmentDescription������һ����ɫ�������ģ�帽�ŵĽṹ����������һ�������ĸ��ţ�����һ�ֵ���������
	VkAttachmentDescription1 VkAttachmentDescription2 VkAttachmentDescription3 VkAttachmentDescription4�����ģ�壩��������磬��Ҫʲô
	VkAttachmentReference ˵������SubPass ��Ҫ��attachment���е�һ������һ������������id�Ƕ��٣���һ�����ŵ�ͼƬ������ʽ��ʲô
	VkSubpassDescription������дһ����Pass�������ṹ
	VkSubpassDependency��������ͬ��������֮���������ϵ
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