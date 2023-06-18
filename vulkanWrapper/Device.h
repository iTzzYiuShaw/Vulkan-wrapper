//
// Created by Shawwy on 6/16/2023.
//

#ifndef INDIVIDUALPROJECT_DEVICE_H
#define INDIVIDUALPROJECT_DEVICE_H

#include "../base.h"
#include "Instance.h"

namespace IP::Wrapper{

    class Device {

    public:
        using Ptr = std::shared_ptr<Device>;
        static Ptr create(Instance::Ptr instance) {return std::make_shared<Device>(instance);}

        Device(Instance::Ptr instance);
        ~Device();

        void pickPhysicalDevice();

        int rateDevice(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        void initQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

    private:
        VkPhysicalDevice  mPhysicalDevice{VK_NULL_HANDLE};
        Instance::Ptr mInstance{nullptr};

        //Store the id of graphic queue family
        std::optional<uint32_t> mGraphicQueueFamily;
        VkQueue mGraphicQueue {VK_NULL_HANDLE};

        //Logic Device
        VkDevice mDevice{VK_NULL_HANDLE};
    };

}


#endif //INDIVIDUALPROJECT_DEVICE_H
