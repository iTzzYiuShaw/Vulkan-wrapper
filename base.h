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
#include <set>
#include <optional>
#include <fstream>
#include <array>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

//Enable validation layer
//Can be used in all encapsulated vulkan class
const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};
struct VPMatrices {
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;

    VPMatrices() {
        mViewMatrix = glm::mat4(1.0f);
        mProjectionMatrix = glm::mat4(1.0f);
    }
};

struct ObjectUniform {
    glm::mat4 mModelMatrix;

    ObjectUniform() {
        mModelMatrix = glm::mat4(1.0f);
    }
};
#endif //INDIVIDUALPROJECT_BASE_H
