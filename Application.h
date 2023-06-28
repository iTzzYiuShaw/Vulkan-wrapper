//
// Created by Shawwy on 6/13/2023.
//
#pragma once
#ifndef INDIVIDUALPROJECT_APPLICATION_H
#define INDIVIDUALPROJECT_APPLICATION_H

#include "base.h"
#include "vulkanWrapper/Instance.h"
#include "vulkanWrapper/Device.h"
#include "vulkanWrapper/Window.h"
#include "vulkanWrapper/WindowSurface.h"
#include "vulkanWrapper/swapChain.h"
#include "vulkanWrapper/shader.h"
#include "vulkanWrapper/pipline.h"
#include "vulkanWrapper/commandBuffer.h"
#include "vulkanWrapper/commandPool.h"
#include "vulkanWrapper/semaphore.h"
#include "vulkanWrapper/fence.h"
#include "vulkanWrapper/buffer.h"
#include "vulkanWrapper/descriptorSetLayout.h"
#include "vulkanWrapper/descriptorPool.h"
#include "vulkanWrapper/descriptorSet.h"
#include "vulkanWrapper/description.h"
#include "uniformManager.h"
#include "model.h"

namespace IP
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    class Application {
    public:
        Application() = default;

        ~Application() = default;

        void run();

    private:
        void initWindow();

        void initVulkan();

        void mainLoop();

        void render();

        void cleanUp();

    private:
        unsigned int mWidth{ 800 };
        unsigned int mHeight{ 600 };

    private:
        void createPipeline();
        void createRenderPass();

        void createCommandBuffers();
        void createSyncObjects();

        void recreateSwapChain();
        void cleanupSwapChain();

    private:
        int mCurrentFrame{ 0 };
        Wrapper::Window::Ptr mWindow{ nullptr };
        Wrapper::Instance::Ptr mInstance{ nullptr };
        Wrapper::Device::Ptr mDevice{ nullptr };
        Wrapper::WindowSurface::Ptr mSurface{ nullptr };
        Wrapper::SwapChain::Ptr mSwapChain{ nullptr };
        Wrapper::Pipeline::Ptr mPipeline{ nullptr };
        Wrapper::RenderPass::Ptr mRenderPass{nullptr};
        Wrapper::CommandPool::Ptr mCommandPool{ nullptr };

        std::vector<Wrapper::CommandBuffer::Ptr> mCommandBuffers{};
        std::vector<Wrapper::Semaphore::Ptr> mImageAvailableSemaphores{};
        std::vector<Wrapper::Semaphore::Ptr> mRenderFinishedSemaphores{};
        std::vector<Wrapper::Fence::Ptr> mFences{};

        UniformManager::Ptr mUniformManager{ nullptr };

        Model::Ptr	mModel{ nullptr };
        VPMatrices	mVPMatrices;
    };
}



#endif //INDIVIDUALPROJECT_APPLICATION_H
