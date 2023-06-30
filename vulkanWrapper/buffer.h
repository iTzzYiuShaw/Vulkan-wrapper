#pragma once

#include "../base.h"
#include "Device.h"

namespace IP::Wrapper {

	class Buffer {
	
	public:
		using Ptr = std::shared_ptr<Buffer>;
		static Ptr create(const Device::Ptr& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) { 
			return std::make_shared<Buffer>(device, size, usage, properties); 
		}

	public:
		static Ptr createVertexBuffer(const Device::Ptr& device, VkDeviceSize size, void * pData);

		static Ptr createIndexBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData);

		static Ptr createUniformBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData = nullptr);

		static Ptr createStageBuffer(const Device::Ptr& device, VkDeviceSize size, void* pData = nullptr);

	public:
		Buffer(const Device::Ptr &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		~Buffer();

        /**
          * 1: By memory mapping, we can directly modify the memory, which is suitable for host-visible types of memory.
          * 2: If this memory is local-optimap, we must create an intermediate stage buffer, copy it to the stage buffer first, and then copy it to the target buffer
          */

		void updateBufferByMap(void *data, size_t size);

		void updateBufferByStage(void* data, size_t size);

		void copyBuffer(const VkBuffer& srcBuffer, const VkBuffer& dstBuffer, VkDeviceSize size);

		[[nodiscard]] auto getBuffer() const { return mBuffer; }

		[[nodiscard]] VkDescriptorBufferInfo& getBufferInfo() { return mBufferInfo; }
		
	private:
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	private:
		VkBuffer mBuffer{ VK_NULL_HANDLE };
		VkDeviceMemory mBufferMemory{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
		VkDescriptorBufferInfo mBufferInfo{};
	};
}