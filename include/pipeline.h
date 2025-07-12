#ifndef PIPELINE_H
#define PIPELINE_H

#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "helper.h"

// -----------------------------------------------------------------------------
// Creates a simple render pipeline for drawing a single triangle.
//
// Parameters:
//   device               - The WebGPU device used to create the pipeline
//   surfaceTextureFormat - The format of the surface to render into (e.g., BGRA8Unorm)
//   shader               - A shader module containing both vertex and fragment entry points
//
// Returns:
//   A configured wgpu::RenderPipeline ready for rendering
// -----------------------------------------------------------------------------
wgpu::RenderPipeline createTrianglePipeline(wgpu::Device device, wgpu::TextureFormat surfaceTextureFormat, wgpu::ShaderModule shader) {
    wgpu::RenderPipelineDescriptor pipelineDesc = {};
    pipelineDesc.label = toStringView("Render Pipeline");

    // ----------------------------------------
    // Vertex Stage Configuration
    // ----------------------------------------

    // No vertex buffers (hardcoded in shader)
    pipelineDesc.vertex.bufferCount = 0;
    pipelineDesc.vertex.buffers = nullptr;

    // Vertex shader module and entry point
    pipelineDesc.vertex.module = shader;
    pipelineDesc.vertex.entryPoint = toStringView("vs_main");
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;

    // ----------------------------------------
    // Primitive State (how to interpret vertices)
    // ----------------------------------------
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList; // 3 vertices per triangle
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;  // No index buffer
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;                 // Counter-clockwise is front
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;                  // No face culling

    // ----------------------------------------
    // Fragment Stage Configuration
    // ----------------------------------------
    wgpu::FragmentState fragmentState = {};
    fragmentState.module = shader;
    fragmentState.entryPoint = toStringView("fs_main");
    fragmentState.constantCount = 0;
    fragmentState.constants = nullptr;

    // Configure blending for color output (standard alpha blending)
    wgpu::BlendState blendState = {};
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;

    blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blendState.alpha.dstFactor = wgpu::BlendFactor::One;
    blendState.alpha.operation = wgpu::BlendOperation::Add;

    // Define the color target format and attach the blend state
    wgpu::ColorTargetState colorTargetState = {};
    colorTargetState.format = surfaceTextureFormat;
    colorTargetState.blend = &blendState;
    colorTargetState.writeMask = wgpu::ColorWriteMask::All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;

    pipelineDesc.fragment = &fragmentState;

    // ----------------------------------------
    // No Depth/Stencil Configuration
    // ----------------------------------------
    pipelineDesc.depthStencil = nullptr; // No depth buffer needed for a single triangle

    // ----------------------------------------
    // Multisampling Configuration
    // ----------------------------------------
    pipelineDesc.multisample.count = 1;                     // No MSAA
    pipelineDesc.multisample.mask = ~0u;                    // All samples enabled
    pipelineDesc.multisample.alphaToCoverageEnabled = false; // Not used

    // ----------------------------------------
    // Use auto layout (no bind groups)
    // ----------------------------------------
    pipelineDesc.layout = nullptr;

    // ----------------------------------------
    // Create and return the pipeline
    // ----------------------------------------
    return device.createRenderPipeline(pipelineDesc);
}

#endif // PIPELINE_H
