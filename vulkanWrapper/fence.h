#pragma once

#include "../base.h"
#include "device.h"

namespace IP::Wrapper {
    /**
       * A fence is a synchronization primitive used to manage a batch submission to a queue.
       * This is different from a semaphore, which manages dependencies between different execution stages within a single command submission.
       * A fence controls all commands submitted at once to a queue. Fences have two states, signaled and unsignaled, and can be controlled at the API level
       */
	class Fence {
	public:
		using Ptr = std::shared_ptr<Fence>;
		static Ptr create(const Device::Ptr& device, bool signaled = true) {
			return std::make_shared<Fence>(device, signaled);
		}

		Fence(const Device::Ptr& device, bool signaled = true);

		~Fence();

        //Set the fence to be unsignaled
		void resetFence();

        //If the fence is unsignaled, then the program will be blocked.
		void block(uint64_t timeout = UINT64_MAX);

		[[nodiscard]] auto getFence() const { return mFence; }
	private:
		VkFence mFence{ VK_NULL_HANDLE };
		Device::Ptr mDevice{ nullptr };
	};
}