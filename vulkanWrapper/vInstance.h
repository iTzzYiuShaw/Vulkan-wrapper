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
        bool checkValidationLayerSupport();


        std::vector<const char*> getRequiredExtensions();
    private:
        VkInstance mInstance;
        bool mEnableValidationLayer{false};
    private:

    };
}



#endif //INDIVIDUALPROJECT_VINSTANCE_H
