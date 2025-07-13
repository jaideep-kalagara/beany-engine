#ifndef RESOURCES_H
#define RESOURCES_H

#include <webgpu/webgpu-raii.hpp>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "helper.h"

Geometry loadGeometry2D(std::filesystem::path path, wgpu::Device& device) {
    Geometry geometry;

    std::vector<float> points;
    std::vector<uint16_t> indices;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return geometry;
    }

    enum class Section {
        None,
        Points,
        Indices
    };

    Section currentSection = Section::None;

    float value;
    uint16_t index;
    std::string line;

    while (!file.eof()) {
        std::getline(file, line);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line == "[points]") {
            currentSection = Section::Points;
        } else if (line == "[indices]") { 
            currentSection = Section::Indices;
        } else if (line[0] == '#' || line.empty()) { // comment line or empty
            continue;
        } else if (currentSection == Section::Points) {
            std::istringstream iss(line);
            for (int i = 0; i < 5; i++) {
                iss >> value;
                points.push_back(value);
            }
        } else if (currentSection == Section::Indices) {
            std::istringstream iss(line);
            for (int i = 0; i < 3; i++) {
                iss >> index;
                indices.push_back(index);
            }
        } 
    }

    file.close();

    std::cout << "Loaded " << points.size() << " points and " << indices.size() << " indices" << std::endl;

    // create vertex buffer
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = points.size() * sizeof(float);

    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;
    

    geometry.points = device.createBuffer(bufferDesc);
    device.getQueue().writeBuffer(geometry.points, 0, points.data(), bufferDesc.size);

    // create index buffer
    indices.resize((indices.size() + 1) & ~1);
    wgpu::BufferDescriptor indexBufferDesc;
    indexBufferDesc.size = indices.size() * sizeof(uint16_t);
    indexBufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Index;
    indexBufferDesc.mappedAtCreation = false;

    geometry.indices = device.createBuffer(indexBufferDesc);
    device.getQueue().writeBuffer(geometry.indices, 0, indices.data(), indexBufferDesc.size);

    return geometry;
}

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
#endif // RESOURCES_H