//
// Created by Shawwy on 6/16/2023.
//
#include "Device.h"

namespace IP::Wrapper
{
    Device::Device(Instance::Ptr instance,WindowSurface::Ptr surface) {
        mInstance = instance;
        mSurface = surface;
        pickPhysicalDevice();
        initQueueFamilies(mPhysicalDevice);
        createLogicalDevice();
    }


    Device::~Device() {
        vkDestroyDevice(mDevice, nullptr);
        mSurface.reset();
        mInstance.reset();
    }

    /*
     * creating a logical device and acquiring the necessary queues for graphics rendering and presentation operations.
     * */
    void Device::createLogicalDevice() {

        //requests for one queue from each queue family and assigns the maximum priority 1.0.
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> queueFamilies = {mGraphicQueueFamily.value(), mPresentQueueFamily.value()};
        float queuePriority = 1.0;


        /**
         * Fill the queueCreateInfo for each unique queue family
         * If the indices for the graphics queue family and the presentation queue family are the same (which means both operations can be done in the same queue family),
         * the queueFamilies set will contain only one unique element, and the loop will iterate only once.
         * If the indices for the graphics queue family and the presentation queue family are different (which means the operations require separate queue families),
         * the queueFamilies set will contain two unique elements, and the loop will iterate twice.
         */
        for (uint32_t queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        //TODO:can be used to enable different features
        VkPhysicalDeviceFeatures deviceFeatures = {};

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceRequiredExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();


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

        /**
         * Retrieves the handles of the graphics queue and the presentation queue from the logical device using the vkGetDeviceQueue function.
         * The queues are stored in mGraphicQueue and mPresentQueue respectively.
         */
        vkGetDeviceQueue(mDevice,mGraphicQueueFamily.value(),0, &mGraphicQueue);
        vkGetDeviceQueue(mDevice,mPresentQueueFamily.value(),0,&mPresentQueue);
    }

    void Device::initQueueFamilies(VkPhysicalDevice device) {
        uint32_t queueCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device,&queueCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device,&queueCount,queueFamilies.data());

        int i = 0;

        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
                mGraphicQueueFamily = i;
            }

            //check if this queue family supports presenting on surface
            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface->getSurface(), &presentSupport);

            if (presentSupport) {
                mPresentQueueFamily = i;
            }

            if (isQueueFamilyComplete()) {
                break;
            }

            ++i;
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

    bool Device::isQueueFamilyComplete() {
        return mGraphicQueueFamily.has_value() && mPresentQueueFamily.has_value();
    }
}