//
// Created by Shawwy on 6/16/2023.
//

#ifndef INDIVIDUALPROJECT_VDEVICE_H
#define INDIVIDUALPROJECT_VDEVICE_H

#include "../base.h"
#include "vInstance.h"

namespace IP::Wrapper{

    class vDevice {

    public:
        using Ptr = std::shared_ptr<vDevice>;
        static Ptr create(vInstance::Ptr instance) {return std::make_shared<vDevice>(instance);}

        vDevice(vInstance::Ptr instance);
        ~vDevice();

        void pickPhysicalDevice();

        int rateDevice(VkPhysicalDevice device);

        bool isDeviceSuitable(VkPhysicalDevice device);

        void initQueueFamilies(VkPhysicalDevice device);

        void createLogicalDevice();

    private:
        VkPhysicalDevice  mPhysicalDevice{VK_NULL_HANDLE};
        vInstance::Ptr mInstance{nullptr};

        //Store the id of graphic queue family
        std::optional<uint32_t> mGraphicQueueFamily;
        VkQueue mGraphicQueue {VK_NULL_HANDLE};

        //Logic device
        VkDevice mDevice{VK_NULL_HANDLE};
    };

}


#endif //INDIVIDUALPROJECT_VDEVICE_H
