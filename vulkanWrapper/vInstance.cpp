//
// Created by Shawwy on 6/13/2023.
//

#include "vInstance.h"

namespace IP::Wrapper
{
    //Enable validation layer
    const std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };


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
    }

    vInstance::~vInstance(){
        vkDestroyInstance(mInstance, nullptr);
        std::cout << "Destroy instance" << std::endl;
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
}