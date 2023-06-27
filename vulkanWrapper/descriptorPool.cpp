#include "descriptorPool.h"

namespace IP::Wrapper {

	DescriptorPool::DescriptorPool(const Device::Ptr &device) {
		mDevice = device;
	}

	DescriptorPool::~DescriptorPool() {
		if (mPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(mDevice->getDevice(), mPool, nullptr);
		}
	} 

	void DescriptorPool::build(std::vector<UniformParameter::Ptr>& params, const int& frameCount) {
		//decriptor
		//descriptorSet(decriptorA(Buffer), decriptorA, decriptorB)
		//descriptorSet * N ��Ϊ�����������У�����buffer,��ǰһ֡�ύ��ʱ��������֡���ڻ��Ƶ��У�
		//����uniformbuffer���������ڱ���ȡ����ʱcpu�˵���һ��ѭ����ȴ������������ݵ��޸�

		int uniformBufferCount = 0;
		//TODO: ������������uniform�ж��ٸ���

		for (const auto& param : params) {
			if (param->mDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) { uniformBufferCount++; }
			//TODO
		}

		//����ÿһ��uniform���ж��ٸ�
		std::vector<VkDescriptorPoolSize> poolSizes{};

		VkDescriptorPoolSize uniformBufferSize{};
		uniformBufferSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformBufferSize.descriptorCount = uniformBufferCount * frameCount;
		poolSizes.push_back(uniformBufferSize);

		//����pool
		VkDescriptorPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		createInfo.pPoolSizes = poolSizes.data();
		createInfo.maxSets = static_cast<uint32_t>(frameCount);

		if (vkCreateDescriptorPool(mDevice->getDevice(), &createInfo, nullptr, &mPool) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create Descriptor pool");
		}
	}

}