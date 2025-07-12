#ifndef SHADERS_H
#define SHADERS_H

#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "helper.h"

// -----------------------------------------------------------------------------
// Loads a WGSL shader file from disk and creates a WebGPU shader module.
// 
// Parameters:
//   path   - Path to the .wgsl shader source file
//   device - WebGPU device used to create the shader module
//
// Returns:
//   A compiled wgpu::ShaderModule object ready for use in pipelines
// -----------------------------------------------------------------------------
wgpu::ShaderModule createShaderModuleWGSL(const std::string& path, wgpu::Device device) {
    // Open the WGSL file
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    // Read entire file contents into a string
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileData = buffer.str();
    file.close();

    // ----------------------------------------
    // Describe shader source (WGSL format)
    // ----------------------------------------
    wgpu::ShaderSourceWGSL shaderSourceWGSL;
    shaderSourceWGSL.chain.next = nullptr;
    shaderSourceWGSL.chain.sType = wgpu::SType::ShaderSourceWGSL;
    shaderSourceWGSL.code = toStringView(fileData); // Convert to WebGPU-compatible string view

    // ----------------------------------------
    // Create the shader module descriptor
    // ----------------------------------------
    wgpu::ShaderModuleDescriptor descriptor = {};
    descriptor.nextInChain = &shaderSourceWGSL.chain;

    // ----------------------------------------
    // Create and return the shader module
    // ----------------------------------------
    return device.createShaderModule(descriptor);
}


#endif // SHADERS_H
