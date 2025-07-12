#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "callbacks.h"
#include "app.h"
#include "helper.h"
#include "renderer.h"
#include "surface.h"
#include "pipeline.h"
#include "shaders.h"

// ---------------------------------------------------------
// Initializes GPU vertex buffer with interleaved position/color data
// ---------------------------------------------------------
void Application::initializeBuffers() {
    std::vector<float> vertexData = {
        // x, y,    r, g, b
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // Red
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Green
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f  // Blue
    };

    size_t bufferSize = vertexData.size() * sizeof(float);

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = bufferSize;
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex;
    bufferDesc.mappedAtCreation = false;

    vertexBuffer = device.createBuffer(bufferDesc);
    queue.writeBuffer(vertexBuffer, 0, vertexData.data(), bufferSize);
}

// ---------------------------------------------------------
// Initializes GLFW, WebGPU instance, device, surface, pipeline
// ---------------------------------------------------------
bool Application::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, "WebGPU Beany Renderer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    wgpu::Instance instance = wgpu::createInstance();
    surface = getSurface(window, instance);
    if (!surface) {
        std::cerr << "Failed to create surface\n";
        return false;
    }

    wgpu::Adapter adapter = getAdapter(instance, surface);
    if (!adapter) {
        std::cerr << "Failed to request adapter\n";
        return false;
    }

    wgpu::AdapterInfo adapterInfo;
    adapter.getInfo(&adapterInfo);
    std::cout << "Using device: " << adapterInfo.device.data << "\n";

    device = getDevice(adapter);
    if (!device) {
        std::cerr << "Failed to request device\n";
        return false;
    }

    queue = device.getQueue();

    wgpu::QueueWorkDoneCallbackInfo cbInfo = {};
    cbInfo.callback = queueWorkDoneCallback;
    queue.onSubmittedWorkDone(cbInfo);

    configureSurface(surface, adapter, device);

    wgpu::ShaderModule triangleShader = createShaderModuleWGSL("assets/shaders/triangle.wgsl", device);

    wgpu::SurfaceCapabilities capabilities;
    surface.getCapabilities(adapter, &capabilities);
    pipeline = createTrianglePipeline(device, capabilities.formats[0], triangleShader);

    initializeBuffers();
    return true;
}

// ---------------------------------------------------------
// Runs the main rendering loop
// ---------------------------------------------------------
void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        auto [surfaceTexture, targetView] = getNextSurfaceViewData(surface);
        if (!targetView) break;

        wgpu::CommandEncoderDescriptor encoderDesc = {};
        encoderDesc.label = toStringView("Encoder");
        wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);

        wgpu::RenderPassColorAttachment colorAttachment = clearColorAttachment(targetView, { 0.0f, 0.0f, 0.0f, 1.0f });
        wgpu::RenderPassDescriptor passDesc = { };

        passDesc.colorAttachmentCount = 1;
        passDesc.colorAttachments = &colorAttachment;

        wgpu::RenderPassEncoder pass = encoder.beginRenderPass(passDesc);
        pass.setPipeline(pipeline);
        pass.setVertexBuffer(0, vertexBuffer, 0, vertexBuffer.getSize());
        pass.draw(vertexBuffer.getSize() / sizeof(float) / 5, 1, 0, 0);
        pass.end();

        wgpu::CommandBuffer commands = encoder.finish();
        queue.submit(1, &commands);
        surface.present();

        glfwPollEvents();
        device.poll(false, nullptr);
    }
}

// ---------------------------------------------------------
// Cleans up resources
// ---------------------------------------------------------
void Application::terminate() {
    surface.unconfigure();
    glfwDestroyWindow(window);
    glfwTerminate();
}
