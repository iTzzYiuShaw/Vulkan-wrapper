//
// Created by Shawwy on 6/16/2023.
//

#ifndef INDIVIDUALPROJECT_DEVICE_H
#define INDIVIDUALPROJECT_DEVICE_H

#include "../base.h"
#include "instance.h"

namespace IP::Wrapper{

    class device {

    public:
        using Ptr = std::shared_ptr<device>;
        static Ptr create(instance::Ptr instance) {return std::make_shared<device>(instance);}

        device(instance::Ptr instance);
        ~device();

        void pickPhysicalDevice();

        int rateDevice(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        void initQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

    private:
        VkPhysicalDevice  mPhysicalDevice{VK_NULL_HANDLE};
        instance::Ptr mInstance{nullptr};

        //Store the id of graphic queue family
        std::optional<uint32_t> mGraphicQueueFamily;
        VkQueue mGraphicQueue {VK_NULL_HANDLE};

        //Logic device
        VkDevice mDevice{VK_NULL_HANDLE};
    };

}


#endif //INDIVIDUALPROJECT_DEVICE_H
