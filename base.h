//
// Created by Shawwy on 6/13/2023.
//

#ifndef INDIVIDUALPROJECT_BASE_H
#define INDIVIDUALPROJECT_BASE_H
#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <optional>
#include <glm/glm.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//Enable validation layer
//Can be used in all encapsulated vulkan class
const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

#endif //INDIVIDUALPROJECT_BASE_H
