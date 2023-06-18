//
// Created by Shawwy on 6/16/2023.
//
#include "Device.h"

namespace IP::Wrapper
{
    Device::Device(Instance::Ptr instance) {
        mInstance = instance;
        pickPhysicalDevice();
        initQueueFamilies(mPhysicalDevice);
        createLogicalDevice();
    }


    Device::~Device() {
        vkDestroyDevice(mDevice, nullptr);
        mInstance.reset();
    }


    void Device::createLogicalDevice() {

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = mGraphicQueueFamily.value();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        //TODO:can be used to enable different features
        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = 0;


        if (mInstance->getEnableValidationLayer()) {
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            deviceCreateInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
            throw std::runtime_error("Error:failed to create logical Device");
        }

        vkGetDeviceQueue(mDevice, mGraphicQueueFamily.value(), 0, &mGraphicQueue);
    }

    void Device::initQueueFamilies(VkPhysicalDevice device) {
        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device,&queueCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device,&queueCount,queueFamilies.data());

        int graphicQueueId = 0;

        for (const auto& queueFamily:queueFamilies) {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                mGraphicQueueFamily = graphicQueueId;
            }
            if (mGraphicQueueFamily.has_value())
            {
                break;
            }
            ++graphicQueueId;
        }
    }

    void Device::pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance->getInstance(),&deviceCount, nullptr);

        if (deviceCount == 0){
            throw std::runtime_error("Error: failed to enumeratePhysicalDevices");
        }

        std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
        vkEnumeratePhysicalDevices(mInstance->getInstance(),&deviceCount,physicalDevices.data());

        std::multimap<int,VkPhysicalDevice> candidates;
        for (const auto& device: physicalDevices) {
            int score = rateDevice(device);
            candidates.insert(std::make_pair(score,device));
        }

        if (candidates.rbegin()->first > 0 && isDeviceSuitable(candidates.rbegin()->second))
        {
            mPhysicalDevice = candidates.rbegin()->second;
        }

        if (mPhysicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("Error: failed to get PhysicalDevices");
        }
    }

    int Device::rateDevice(VkPhysicalDevice device) {
        int score = 0;

        //Acquire Device properties, for example: name and type of the Device, supported version of Vulkan etc.
        VkPhysicalDeviceProperties deviceProp;
        vkGetPhysicalDeviceProperties(device,&deviceProp);

        //Acquire Device features, for example: texture compression
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        score += deviceProp.limits.maxImageDimension2D;

        if (!deviceFeatures.geometryShader)
            return 0;

        return score;
    }

    bool Device::isDeviceSuitable(VkPhysicalDevice device) {
        //Acquire Device properties, for example: name and type of the Device, supported version of Vulkan etc.
        VkPhysicalDeviceProperties deviceProp;
        vkGetPhysicalDeviceProperties(device,&deviceProp);

        //Acquire Device features, for example: texture compression
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    }
}