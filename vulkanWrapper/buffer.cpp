#include "buffer.h"
#include "commandBuffer.h"
#include"commandPool.h"

namespace IP::Wrapper {
	Buffer::Ptr Buffer::createVertexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(
			device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		buffer->updateBufferByStage(pData, size);

		return buffer;
	}

	Buffer::Ptr Buffer::createIndexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
		auto buffer = Buffer::create(
			device, size,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		buffer->updateBufferByStage(pData, size);

		return buffer;
	}

    Buffer::Ptr Buffer::createUniformBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData) {
        auto buffer = Buffer::create(
                device, size,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        if (pData != nullptr) {
            buffer->updateBufferByStage(pData, size);
        }

        return buffer;
    }

	Buffer::Buffer(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		mDevice = device;

		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(mDevice->getDevice(), &createInfo, nullptr, &mBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Error:failed to create buffer");
		}

		//Creating a device memory space
		VkMemoryRequirements memReq{};
		vkGetBufferMemoryRequirements(mDevice->getDevice(), mBuffer, &memReq);

        //Type and the size of the memory
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memReq.size;

		//����������buffer������ڴ����͵�ID�ǣ�0x001 0x010
		allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);

		if (vkAllocateMemory(mDevice->getDevice(), &allocInfo, nullptr, &mBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to allocate memory");
		}

		vkBindBufferMemory(mDevice->getDevice(), mBuffer, mBufferMemory, 0);
	}

	Buffer::~Buffer() {
		if (mBuffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(mDevice->getDevice(), mBuffer, nullptr);
		}

		if (mBufferMemory != VK_NULL_HANDLE) {
			vkFreeMemory(mDevice->getDevice(), mBufferMemory, nullptr);
		}
	}

    /**
     *
     * @param typeFilter is a bit mask where each bit corresponds to a possible memory type. If a bit in the mask is set (1),
     * then the corresponding memory type is considered acceptable; if it is not set (0), then the corresponding memory type is not considered.
     * @param properties  is a set of flags representing the properties that the required memory type should have.
     * @return index of this memory type
     * This piece of code is used to find a memory type that fulfills certain property requirements.
     * The body of the function is a loop, checking each potential memory type.
     * If a memory type is within the filter set by typeFilter and its properties meet the requirements set by properties, then the function returns the index of this memory type.
     *
     */
	uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(mDevice->getPhysicalDevice(), &memProps);

		// if 0x001 | 0x100 = 0x101  i = 0 ;���  1 << i 1   i = 1 0x010
        /**
         * The & operator here is the bitwise AND operator.
         * It compares each bit of typeFilter to the corresponding bit of (1 << i).
         * If both bits are 1, the corresponding result bit is set to 1. Otherwise, the result bit is set to 0.
         */
		for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i)) && ((memProps.memoryTypes[i].propertyFlags & properties) == properties)) {
				return i;
			}
		}

		throw std::runtime_error("Error: cannot find the property memory type!");
	}

	void Buffer::updateBufferByMap(void* data, size_t size) {

		void* memPtr = nullptr;
		vkMapMemory(mDevice->getDevice(), mBufferMemory, 0, size, 0, &memPtr);
		memcpy(memPtr, data, size);
		vkUnmapMemory(mDevice->getDevice(), mBufferMemory);
	}

	void Buffer::updateBufferByStage(void* data, size_t size) {
		auto stageBuffer = Buffer::create(mDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stageBuffer->updateBufferByMap(data, size);

		copyBuffer(stageBuffer->getBuffer(), mBuffer, static_cast<VkDeviceSize>(size));
	}

	void Buffer::copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size) {
		auto commandPool = CommandPool::create(mDevice);
		auto commandBuffer = CommandBuffer::create(mDevice, commandPool);

		commandBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkBufferCopy copyInfo{};
		copyInfo.size = size;
		
		commandBuffer->copyBuffer(srcBuffer, dstBuffer, 1, { copyInfo });

		commandBuffer->end();

		commandBuffer->submitSync(mDevice->getGraphicQueue(), VK_NULL_HANDLE);
	}
}