#ifndef PIPELINE_H
#define PIPELINE_H

#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

#include "helper.h"

// -----------------------------------------------------------------------------
// Creates a simple render pipeline for drawing a single triangle.
// -----------------------------------------------------------------------------
wgpu::RenderPipeline createTrianglePipeline(
    wgpu::Device device,
    wgpu::TextureFormat surfaceTextureFormat,
    wgpu::ShaderModule shader)
{
    wgpu::RenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.label = toStringView("Render Pipeline");

    // ----------------------------------------
    // Vertex Stage Configuration
    // ----------------------------------------
    static wgpu::VertexAttribute attributes[2];

    // Location 0: vec2f position (2 * float)
    attributes[0].shaderLocation = 0;
    attributes[0].offset = 0;
    attributes[0].format = wgpu::VertexFormat::Float32x2;

    // Location 1: vec3f color (3 * float)
    attributes[1].shaderLocation = 1;
    attributes[1].offset = sizeof(float) * 2;
    attributes[1].format = wgpu::VertexFormat::Float32x3;

    static wgpu::VertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.attributeCount = 2;
    vertexBufferLayout.attributes = attributes;
    vertexBufferLayout.arrayStride = sizeof(float) * 5;
    vertexBufferLayout.stepMode = wgpu::VertexStepMode::Vertex;

    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.vertex.module = shader;
    pipelineDesc.vertex.entryPoint = toStringView("vs_main");
    pipelineDesc.vertex.constantCount = 0;
    pipelineDesc.vertex.constants = nullptr;

    // ----------------------------------------
    // Primitive Configuration
    // ----------------------------------------
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    // ----------------------------------------
    // Fragment Stage Configuration
    // ----------------------------------------
    wgpu::FragmentState fragmentState{};
    fragmentState.module = shader;
    fragmentState.entryPoint = toStringView("fs_main");

    static wgpu::BlendState blendState{};
    blendState.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    blendState.alpha.srcFactor = wgpu::BlendFactor::Zero;
    blendState.alpha.dstFactor = wgpu::BlendFactor::One;
    blendState.alpha.operation = wgpu::BlendOperation::Add;

    static wgpu::ColorTargetState colorTargetState{};
    colorTargetState.format = surfaceTextureFormat;
    colorTargetState.blend = &blendState;
    colorTargetState.writeMask = wgpu::ColorWriteMask::All;

    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;

    pipelineDesc.fragment = &fragmentState;

    // ----------------------------------------
    // Depth-Stencil & Multisampling (unused here)
    // ----------------------------------------
    pipelineDesc.depthStencil = nullptr;
    pipelineDesc.multisample.count = 1;
    pipelineDesc.multisample.mask = ~0u;
    pipelineDesc.multisample.alphaToCoverageEnabled = false;

    // ----------------------------------------
    // Use auto layout (no explicit bind groups)
    // ----------------------------------------
    pipelineDesc.layout = nullptr;

    return device.createRenderPipeline(pipelineDesc);
}

#endif // PIPELINE_H
