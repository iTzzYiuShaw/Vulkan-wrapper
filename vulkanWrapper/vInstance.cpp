//
// Created by Shawwy on 6/13/2023.
//

#include "vInstance.h"

namespace IP::Wrapper
{


    //Callbacks of Validation layer

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageTypeFlag,
            const VkDebugUtilsMessengerCallbackDataEXT* pMessageData,
            void *pUserData
            ){
            std::cout << "ValidationLayer: " << pMessageData->pMessage << std::endl;

            //This won't
            return VK_FALSE;
    }

    //Assistant function to create and initialize (VkDebugUtilsMessengerEXT mDebugger)
    //vkCreateDebugUtilsMessengerEXT is not preloaded,
    //Therefore we need to do it by ourselves.
    static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                 const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                 const VkAllocationCallbacks* pAllocator,
                                                 VkDebugUtilsMessengerEXT* debugMessenger){
        //Load the creation function of VkDebugUtilsMessengerEXT
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            return func(instance,pCreateInfo,pAllocator,debugMessenger);
        } else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    static void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                              VkDebugUtilsMessengerEXT debugMessenger,
                                              const VkAllocationCallbacks* pAllocator){
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance,"vkDestroyDebugUtilsMessengerEXT");

        if (func != nullptr)
        {
            return func(instance,debugMessenger,pAllocator);
        }

    }


    vInstance::vInstance(bool enableValidationLayer) :
    mEnableValidationLayer(enableValidationLayer){

        printAvailableExtensions();

        //perform validation layer checking
        if (mEnableValidationLayer && !checkValidationLayerSupport())
            throw std::runtime_error("Error: validation layer is not supported");

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Individual project";
        appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.pEngineName = "NO ENGINE";
        appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instCreateInfo = {};
        instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instCreateInfo.pApplicationInfo = &appInfo;

        //Extensions related
        auto extensions = getRequiredExtensions();
        instCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instCreateInfo.ppEnabledExtensionNames = extensions.data();

        //Layers related
        if (mEnableValidationLayer){
            instCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            instCreateInfo.ppEnabledLayerNames = validationLayers.data();
        }else {
            instCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&instCreateInfo, nullptr,&mInstance) != VK_SUCCESS)
        {
            std::cerr << "Error: failed to create VkInstance" << std::endl;
        }

        setupDebugger();
    }

    vInstance::~vInstance(){
        if (mEnableValidationLayer)
        {
            DestroyDebugUtilsMessengerEXT(mInstance,mDebugger, nullptr);
        }

        vkDestroyInstance(mInstance, nullptr);
    }

    void vInstance::printAvailableExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr,&extensionCount,extensions.data());

        std::cout << "Available extensions: " << std::endl;

        for(const auto& extension : extensions)
        {
            std::cout << extension.extensionName << std::endl;
        }

    }

    std::vector<const char*> vInstance::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;

        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        //TODO: understand this construction
        std::vector<const char*> extensions(glfwExtensions,glfwExtensions + glfwExtensionCount);

        //Enabling validation layers
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


        return extensions;
    }

    bool vInstance::checkValidationLayerSupport() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayer(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount,availableLayer.data());

        for(const auto& layerName : validationLayers)
        {
            bool layerFound = false;
            for (const auto& layerProp: availableLayer) {
                if (std::strcmp(layerName,layerProp.layerName)){
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound)
                return false;
        }
        return true;
    }

    void vInstance::setupDebugger() {

        if (!mEnableValidationLayer)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT|
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr,&mDebugger) != VK_SUCCESS)
        {
            throw  std::runtime_error("Error: failed to create debugger");
        }
    }
}