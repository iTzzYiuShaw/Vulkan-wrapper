#pragma once

#include "../base.h"
#include "device.h"
#include "description.h"

namespace IP::Wrapper {

	class DescriptorPool {
	public:
		using Ptr = std::shared_ptr<DescriptorPool>;
		static Ptr create(const Device::Ptr& device) { return std::make_shared<DescriptorPool>(device); }

		DescriptorPool(const Device::Ptr &device);

		~DescriptorPool();

		void build(std::vector<UniformParameter::Ptr>& params, const int &frameCount);

		[[nodiscard]] auto getPool() const { return mPool; }

	private:
		//��Ҫ֪����ÿһ��uniform���ж��ٸ�������Ϊ��Ԥ������ռ䣬��ν�ռ䲢����uniformbuffer�Ĵ�С
		//���ǣ�����ÿ��uniform��ͬ����ô�������Ͳ�ͬ������˵�ռ�ָ�����������Ĵ�С���̺���ϵͳ�ڲ���
		VkDescriptorPool mPool{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}