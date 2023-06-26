//
// Created by Shawwy on 6/13/2023.
//

#include "Application.h"

namespace IP
{
    void Application::run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanUp();
    }

    void Application::initWindow()
    {
        mWindow = Wrapper::Window::create(WIDTH,HEIGHT);
    }

    void Application::initVulkan() {
        mInstance = Wrapper::Instance::create(true);
        mSurface = Wrapper::WindowSurface::create(mInstance, mWindow);
        mDevice = Wrapper::Device::create(mInstance,mSurface);

        mSwapChain = Wrapper::SwapChain::create(mDevice, mWindow, mSurface);

        mRenderPass = Wrapper::RenderPass::create(mDevice);
        createRenderPass();

        mSwapChain->createFrameBuffers(mRenderPass);

        mPipeline = Wrapper::Pipeline::create(mDevice,mRenderPass);
        createPipeline();

        mCommandPool = Wrapper::CommandPool::create(mDevice);
        mCommandBuffers.resize(mSwapChain->getImageCount());

        for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
            mCommandBuffers[i] = Wrapper::CommandBuffer::create(mDevice, mCommandPool);

            mCommandBuffers[i]->begin();

            VkRenderPassBeginInfo renderBeginInfo{};
            renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderBeginInfo.renderPass = mRenderPass->getRenderPass();
            renderBeginInfo.framebuffer = mSwapChain->getFrameBuffer(i);
            renderBeginInfo.renderArea.offset = {0, 0};
            renderBeginInfo.renderArea.extent = mSwapChain->getExtent();

            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            renderBeginInfo.clearValueCount = 1;
            renderBeginInfo.pClearValues = &clearColor;


            mCommandBuffers[i]->beginRenderPass(renderBeginInfo);

            mCommandBuffers[i]->bindGraphicPipeline(mPipeline->getPipeline());

            mCommandBuffers[i]->draw(3);

            mCommandBuffers[i]->endRenderPass();

            mCommandBuffers[i]->end();
        }

        for (int i = 0; i < mSwapChain->getImageCount(); ++i) {
            auto imageSemaphore = Wrapper::Semaphore::create(mDevice);
            mImageAvailableSemaphores.push_back(imageSemaphore);

            auto renderSemaphore = Wrapper::Semaphore::create(mDevice);
            mRenderFinishedSemaphores.push_back(renderSemaphore);

            auto fence = Wrapper::Fence::create(mDevice);
            mFences.push_back(fence);
        }
    }

    void Application::mainLoop() {
        while(!mWindow->shouldClose())
        {
            mWindow->pollEvents();
            render();
        }

        vkDeviceWaitIdle(mDevice->getDevice());
    }

    void Application::cleanUp() {
        mPipeline.reset();
        mRenderPass.reset();
        mSwapChain.reset();
        mDevice.reset();
        mSurface.reset();
        mInstance.reset();
        mWindow.reset();
    }
    void Application::render() {

        //Waiting for the current commandBuffer to finish execution
        mFences[mCurrentFrame]->block();

        //Get next frame
        uint32_t imageIndex{ 0 };
        vkAcquireNextImageKHR(
                mDevice->getDevice(),
                mSwapChain->getSwapChain(),
                UINT64_MAX,
                mImageAvailableSemaphores[mCurrentFrame]->getSemaphore(),
                VK_NULL_HANDLE,
                &imageIndex);

        //create submit information
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;


        //Synchronize information. Building the dependencies for the render pass to display the image;
        // color output can only be made after the display is complete
        VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame]->getSemaphore() };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        //ָAcquire command buffer
        auto commandBuffer = mCommandBuffers[imageIndex]->getCommandBuffer();
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame]->getSemaphore()};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        mFences[mCurrentFrame]->resetFence();
        if (vkQueueSubmit(mDevice->getGraphicQueue(), 1, &submitInfo, mFences[mCurrentFrame]->getFence()) != VK_SUCCESS) {
            throw std::runtime_error("Error:failed to submit renderCommand");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {mSwapChain->getSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(mDevice->getPresentQueue(), &presentInfo);

        mCurrentFrame = (mCurrentFrame + 1) % mSwapChain->getImageCount();
    }


    void Application::createPipeline() {
        //Setting up view port
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)WIDTH;
        viewport.height = (float)HEIGHT;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = {WIDTH, HEIGHT};

        mPipeline->setViewports({ viewport });
        mPipeline->setScissors({scissor});


        //Create shaders: vertex and fragment shader
        std::vector<Wrapper::Shader::Ptr> shaderGroup{};

        auto shaderVertex = Wrapper::Shader::create(mDevice, "shaders/vs.spv", VK_SHADER_STAGE_VERTEX_BIT, "main");
        shaderGroup.push_back(shaderVertex);

        auto shaderFragment = Wrapper::Shader::create(mDevice, "shaders/fs.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main");
        shaderGroup.push_back(shaderFragment);

        mPipeline->setShaderGroup(shaderGroup);

        //Input state
        mPipeline->mVertexInputState.vertexBindingDescriptionCount = 0;
        mPipeline->mVertexInputState.pVertexBindingDescriptions = nullptr;
        mPipeline->mVertexInputState.vertexAttributeDescriptionCount = 0;
        mPipeline->mVertexInputState.pVertexAttributeDescriptions = nullptr;

        //Primitive Assembly
        mPipeline->mAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        mPipeline->mAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        mPipeline->mAssemblyState.primitiveRestartEnable = VK_FALSE;

        //Rasterization Configuration
        mPipeline->mRasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        mPipeline->mRasterState.polygonMode = VK_POLYGON_MODE_FILL;
        mPipeline->mRasterState.lineWidth = 1.0f;
        mPipeline->mRasterState.cullMode = VK_CULL_MODE_BACK_BIT;
        mPipeline->mRasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;

        mPipeline->mRasterState.depthBiasEnable = VK_FALSE;
        mPipeline->mRasterState.depthBiasConstantFactor = 0.0f;
        mPipeline->mRasterState.depthBiasClamp = 0.0f;
        mPipeline->mRasterState.depthBiasSlopeFactor = 0.0f;

        //TODO:Multi-sampling
        mPipeline->mSampleState.sampleShadingEnable = VK_FALSE;
        mPipeline->mSampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        mPipeline->mSampleState.minSampleShading = 1.0f;
        mPipeline->mSampleState.pSampleMask = nullptr;
        mPipeline->mSampleState.alphaToCoverageEnable = VK_FALSE;
        mPipeline->mSampleState.alphaToOneEnable = VK_FALSE;



        VkPipelineColorBlendAttachmentState blendAttachment{};
        blendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                         VK_COLOR_COMPONENT_G_BIT |
                                         VK_COLOR_COMPONENT_B_BIT |
                                         VK_COLOR_COMPONENT_A_BIT;

        blendAttachment.blendEnable = VK_FALSE;
        blendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

        blendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        //Because we may have multiple framebuffer outputs, we may need multiple blendAttachments.
        mPipeline->pushBlendAttachment(blendAttachment);

        /**
         * 1: There are two ways of calculating blend, the first is based on alpha, the second is bit operation;
        2: If logicOp is enabled, the computation based on alpha will malfunction;
        3: The ColorWrite mask is still effective even if logicOp is enabled;
         */
        mPipeline->mBlendState.logicOpEnable = VK_FALSE;
        mPipeline->mBlendState.logicOp = VK_LOGIC_OP_COPY;


        mPipeline->mBlendState.blendConstants[0] = 0.0f;
        mPipeline->mBlendState.blendConstants[1] = 0.0f;
        mPipeline->mBlendState.blendConstants[2] = 0.0f;
        mPipeline->mBlendState.blendConstants[3] = 0.0f;

        //No uniform for now
        mPipeline->mLayoutState.setLayoutCount = 0;
        mPipeline->mLayoutState.pSetLayouts = nullptr;
        mPipeline->mLayoutState.pushConstantRangeCount = 0;
        mPipeline->mLayoutState.pPushConstantRanges = nullptr;

        mPipeline->build();
    }
    void Application::createRenderPass() {

        VkAttachmentDescription attachmentDes{};
        attachmentDes.format = mSwapChain->getFormat();
        attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        mRenderPass->addAttachment(attachmentDes);

        VkAttachmentReference attachmentRef{};
        attachmentRef.attachment = 0;
        attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        //Create subpass
        Wrapper::SubPass subPass{};
        subPass.addColorAttachmentReference(attachmentRef);
        subPass.buildSubPassDescription();

        mRenderPass->addSubPass(subPass);

        //Define dependencies between subpasses
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        mRenderPass->addDependency(dependency);

        mRenderPass->buildRenderPass();
    }
}