//
// Created by Shawwy on 6/13/2023.
//
#pragma once
#ifndef INDIVIDUALPROJECT_VINSTANCE_H
#define INDIVIDUALPROJECT_VINSTANCE_H

#include "../base.h"

namespace IP::Wrapper{

    class vInstance {

    public:

        using Ptr = std::shared_ptr<vInstance>;
        static Ptr create(bool enableValidationLayer) {return std::make_shared<vInstance>(enableValidationLayer);}

        vInstance(bool enableValidationLayer);
        ~vInstance();


        void printAvailableExtensions();

        //Layers
        bool checkValidationLayerSupport();
        void setupDebugger();

        //Getter
        std::vector<const char*> getRequiredExtensions();

       [[nodiscard]] VkInstance getInstance() const {return mInstance;};
       [[nodiscard]] bool getEnableValidationLayer() const { return mEnableValidationLayer; }

    private:
        VkInstance mInstance{VK_NULL_HANDLE};
        bool mEnableValidationLayer{false};
        VkDebugUtilsMessengerEXT mDebugger{VK_NULL_HANDLE};
    private:

    };
}



#endif //INDIVIDUALPROJECT_VINSTANCE_H
