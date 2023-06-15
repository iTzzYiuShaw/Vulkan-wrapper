//
// Created by Shawwy on 6/13/2023.
//
#pragma once
#ifndef INDIVIDUALPROJECT_VINSTANCE_H
#define INDIVIDUALPROJECT_VINSTANCE_H

#include "../base.h"
#include <vector>
namespace IP::Wrapper{

    class vInstance {

    public:

        using Ptr = std::shared_ptr<vInstance>;
        static Ptr create() {return std::make_shared<vInstance>();}

        vInstance();
        ~vInstance();

        void printAvailableExtensions();

        std::vector<const char*> getRequiredExtensions();
    private:
        VkInstance mInstance;

    private:

    };
}



#endif //INDIVIDUALPROJECT_VINSTANCE_H
