#include "swapChain.h"

namespace IP::Wrapper {

	SwapChain::SwapChain(const Device::Ptr& device, const Window::Ptr& window, const WindowSurface::Ptr& surface) {
		mDevice = device;
		mWindow = window;
		mSurface = surface;

		auto swapChainSupportInfo = querySwapChainSupportInfo();

		//Choose vkformat
		VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupportInfo.mFormats);

		//Choose presentMode
		VkPresentModeKHR presentMode = chooseSurfacePresentMode(swapChainSupportInfo.mPresentModes);

		//Choose extent
		VkExtent2D extent = chooseExtent(swapChainSupportInfo.mCapabilities);

		//Setting the number of image buffers
		mImageCount = swapChainSupportInfo.mCapabilities.minImageCount + 1;


		//If the maximum image capability exists and the mImageCount is more than this number,
        //we set the mImageCount as maximum image capability
		if (swapChainSupportInfo.mCapabilities.maxImageCount > 0 && mImageCount > swapChainSupportInfo.mCapabilities.maxImageCount) {
			mImageCount = swapChainSupportInfo.mCapabilities.maxImageCount;
		}


		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface->getSurface();
		createInfo.minImageCount = mImageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		/**
		 * Because the images in the swapchain are used for rendering or presentation, and rendering and presentation use different queues,
		 * it's possible for two queues to use the same swapchain.
		 * Therefore, we need to set it so that the swapchain images are compatible with both queues.
		 */

		std::vector<uint32_t> queueFamilies = { mDevice->getGraphicQueueFamily().value() , mDevice->getPresentQueueFamily().value() };

		if (mDevice->getGraphicQueueFamily().value() == mDevice->getPresentQueueFamily().value()) {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//被某一个队列族独占，性能会更好
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;//可以被共享的模式
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());
			createInfo.pQueueFamilyIndices = queueFamilies.data();
		}

		//Initial transform for swapchain image (flip?)
		createInfo.preTransform = swapChainSupportInfo.mCapabilities.currentTransform;

		//No alpha blending between current swapchain image and next swapchain image
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;

		//If some area is blocked by another window, this area won't be rendered
		createInfo.clipped = VK_TRUE;


		if (vkCreateSwapchainKHR(mDevice->getDevice(), &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create swapChain");
		}

		mSwapChainFormat = surfaceFormat.format;
		mSwapChainExtent = extent;


        //Current image count is the minimum image count. The system might create more swap chain images than this
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, nullptr);
		mSwapChainImages.resize(mImageCount);


        //Initialize Vkimage vector
		vkGetSwapchainImagesKHR(mDevice->getDevice(), mSwapChain, &mImageCount, mSwapChainImages.data());

        //Initialize VkImageView vector
		mSwapChainImageViews.resize(mImageCount);
		for (int i = 0; i < mImageCount; ++i) {
			mSwapChainImageViews[i] = createImageView(mSwapChainImages[i], mSwapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
		}
	}

	SwapChain::~SwapChain() {

		for (auto& imageView : mSwapChainImageViews) {
			vkDestroyImageView(mDevice->getDevice(), imageView, nullptr);
		}

        for (auto& frameBuffer : mSwapChainFrameBuffers) {
            vkDestroyFramebuffer(mDevice->getDevice(), frameBuffer, nullptr);
        }

		if (mSwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(mDevice->getDevice(), mSwapChain, nullptr);
		}
		
		mWindow.reset();
		mSurface.reset();
		mDevice.reset();
	}

	SwapChainSupportInfo SwapChain::querySwapChainSupportInfo() {
		SwapChainSupportInfo info;

		//Acquire basic features
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &info.mCapabilities);

		//Acquire surface format that physical device supports
		uint32_t formatCount = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, nullptr);
		if (formatCount != 0) {
			info.mFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &formatCount, info.mFormats.data());
		}

		//Acquire all present modes that physical device supports
		uint32_t presentModeCount = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, nullptr);
		if (presentModeCount != 0) {
			info.mPresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(mDevice->getPhysicalDevice(), mSurface->getSurface(), &presentModeCount, info.mPresentModes.data());
		}

		return info;
	}

	VkSurfaceFormatKHR SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		//If it returns an undefined format, meaning that there is no preferred format. Then we define it manually.
        //The format below is shown in the debug information of querySwapChainSupportInfo() function.
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
			return {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
		}

        //If formats exist, then we iterate all formats, find and return {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::chooseSurfacePresentMode(const std::vector<VkPresentModeKHR>& availablePresenstModes) {

		//Manually set the best mode as VK_PRESENT_MODE_FIFO_KHR
        //VK_PRESENT_MODE_FIFO_KHR is widely supported in physical devices
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        //Iterate and find the best mode among all modes that physical device supports
		for (const auto& availablePresentMode : availablePresenstModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return bestMode;
	}

	VkExtent2D SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

		/*
		 * This design is mainly to handle different windowing systems and platforms.
		 * In some windowing systems, the size of the window might be managed by the windowing system, not controlled by your application.
		 */
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}

		//In some monitors with high resolutions, the size of window is not necessarily equal to the number of pixels
        //To adapt this situation, we will get the window size from glfw window
		int width = 0, height = 0;
		glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));

		actualExtent.height = std::max(capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

	VkImageView SwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;

		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView{ VK_NULL_HANDLE };
		if (vkCreateImageView(mDevice->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("Error: failed to create image view in swapchain");
		}

		return imageView;
	}

    void SwapChain::createFrameBuffers(const RenderPass::Ptr& renderPass) {
        //????FrameBuffer
        mSwapChainFrameBuffers.resize(mImageCount);
        for (int i = 0; i < mImageCount; ++i) {

            std::array<VkImageView, 1> attachments = { mSwapChainImageViews[i] };

            VkFramebufferCreateInfo frameBufferCreateInfo{};
            frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            frameBufferCreateInfo.renderPass = renderPass->getRenderPass();
            frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            frameBufferCreateInfo.pAttachments = attachments.data();
            frameBufferCreateInfo.width = mSwapChainExtent.width;
            frameBufferCreateInfo.height = mSwapChainExtent.height;
            frameBufferCreateInfo.layers = 1;

            if (vkCreateFramebuffer(mDevice->getDevice(), &frameBufferCreateInfo, nullptr, &mSwapChainFrameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Error:Failed to create frameBuffer");
            }
        }
    }
}