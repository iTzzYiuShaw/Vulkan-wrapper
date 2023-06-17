//
// Created by Shawwy on 6/16/2023.
//
#include "vDevice.h"

namespace IP::Wrapper
{
    vDevice::vDevice(vInstance::Ptr instance) {
        mInstance = instance;
        pickPhysicalDevice();
    }


    vDevice::~vDevice() {
        mInstance.reset();
    }

    void vDevice::pickPhysicalDevice() {
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

    int vDevice::rateDevice(VkPhysicalDevice device) {
        int score = 0;

        //Acquire device properties, for example: name and type of the device, supported version of Vulkan etc.
        VkPhysicalDeviceProperties deviceProp;
        vkGetPhysicalDeviceProperties(device,&deviceProp);

        //Acquire device features, for example: texture compression
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

    bool vDevice::isDeviceSuitable(VkPhysicalDevice device) {
        //Acquire device properties, for example: name and type of the device, supported version of Vulkan etc.
        VkPhysicalDeviceProperties deviceProp;
        vkGetPhysicalDeviceProperties(device,&deviceProp);

        //Acquire device features, for example: texture compression
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return deviceProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
    }
}