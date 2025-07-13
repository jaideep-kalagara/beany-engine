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
#include "resources.h"

// ---------------------------------------------------------
// Initializes GPU vertex buffer with interleaved position/color data
// ---------------------------------------------------------
void Application::initializeBuffers() {

    geometry = loadGeometry2D(RESOURCE_DIR "/shape.geo", device);
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
    cbInfo.callback = callbacks::quene::queueWorkDoneCallback;
    queue.onSubmittedWorkDone(cbInfo);

    configureSurface(surface, adapter, device);

    wgpu::ShaderModule triangleShader = createShaderModuleWGSL(RESOURCE_DIR "/shaders/shader.wgsl", device);

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

        // vertex buffer
        pass.setVertexBuffer(0, geometry.points, 0, geometry.points.getSize());

        // index buffer
        pass.setIndexBuffer(geometry.indices, wgpu::IndexFormat::Uint16, 0, geometry.indices.getSize());

        pass.drawIndexed(geometry.indices.getSize() / sizeof(uint16_t), 1, 0, 0, 0);
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
