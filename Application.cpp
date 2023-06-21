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
    }

    void Application::mainLoop() {
        while(!mWindow->shouldClose())
        {
            mWindow->pollEvents();
        }
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