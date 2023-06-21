#pragma once

#include "../base.h"
#include "device.h"

namespace IP::Wrapper {
	/*
	* fence�ǿ���һ�ζ����ύ�ı�־����semaphore����semaphore���Ƶ�һ�����ύ��Ϣ�ڵ�
	* ��ִͬ�н׶�֮���������ϵ��semaphore�޷��ֶ���APIȥ������
	* fence����һ�����У�GraphicQueue������һ�����ύ������ָ��ִ�����
	* ��Ϊ����̬/�Ǽ���̬,���ҿ��Խ���API����Ŀ���
	*/
	class Fence {
	public:
		using Ptr = std::shared_ptr<Fence>;
		static Ptr create(const Device::Ptr& device, bool signaled = true) {
			return std::make_shared<Fence>(device, signaled);
		}

		Fence(const Device::Ptr& device, bool signaled = true);

		~Fence();

		//��Ϊ�Ǽ���̬
		void resetFence();

		//���ô˺��������fenceû�б���������ô����������ȴ�����
		void block(uint64_t timeout = UINT64_MAX);

		[[nodiscard]] auto getFence() const { return mFence; }
	private:
		VkFence mFence{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}