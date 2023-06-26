//
// Created by Shawwy on 6/16/2023.
//

#ifndef INDIVIDUALPROJECT_DEVICE_H
#define INDIVIDUALPROJECT_DEVICE_H

#include "../base.h"
#include "Instance.h"
#include "WindowSurface.h"

namespace IP::Wrapper{

    //Store any extension that are needed in our project
    const std::vector<const char*> deviceRequiredExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    class Device {
    public:
        using Ptr = std::shared_ptr<Device>;
        static Ptr create(Instance::Ptr instance, WindowSurface::Ptr surface) {
            return std::make_shared<Device>(instance, surface);
        }

        Device(Instance::Ptr instance,WindowSurface::Ptr surface);
        ~Device();

        void pickPhysicalDevice();

        int rateDevice(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);
        bool isQueueFamilyComplete();

        void initQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

        //Getter
        [[nodiscard]] auto getDevice() const { return mDevice; }
        [[nodiscard]] auto getPhysicalDevice() const { return mPhysicalDevice; }
        [[nodiscard]] auto getGraphicQueueFamily() const { return mGraphicQueueFamily; }
        [[nodiscard]] auto getPresentQueueFamily() const { return mPresentQueueFamily; }
        [[nodiscard]] auto getGraphicQueue() const { return mGraphicQueue; }
        [[nodiscard]] auto getPresentQueue() const { return mPresentQueue; }
    private:
        VkPhysicalDevice  mPhysicalDevice{VK_NULL_HANDLE};
        Instance::Ptr mInstance{nullptr};
        WindowSurface::Ptr mSurface{ nullptr };

        //Store the id of graphic queue family
        std::optional<uint32_t> mGraphicQueueFamily;
        VkQueue mGraphicQueue {VK_NULL_HANDLE};

        std::optional<uint32_t> mPresentQueueFamily;
        VkQueue mPresentQueue{ VK_NULL_HANDLE };

        //Logic Device
        VkDevice mDevice{VK_NULL_HANDLE};
    };

}


#endif //INDIVIDUALPROJECT_DEVICE_H
