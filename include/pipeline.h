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

    
    wgpu::VertexBufferLayout vertexBufferLayout = {};

    wgpu::VertexAttribute attributes[2];
    // position offset 0, 2 float (vec2f), loc: 0
    attributes[0].shaderLocation = 0;
    attributes[0].offset = 0;
    attributes[0].format = wgpu::VertexFormat::Float32x2;

    // color offset 2 * sizeof(float), 3 float (vec3f / rgb), loc: 1
    attributes[1].shaderLocation = 1;
    attributes[1].offset = 2 * sizeof(float);
    attributes[1].format = wgpu::VertexFormat::Float32x3;

    vertexBufferLayout.attributeCount = 2;
    vertexBufferLayout.attributes = attributes;

    vertexBufferLayout.arrayStride = (2 + 3) * sizeof(float); // 5 values, 2 for pos, 3 for color
    vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex; // one vertex per value 


    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;

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
