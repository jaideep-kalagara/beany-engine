#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

#include "callbacks.h"
#include "app.h"
#include "helper.h"
#include "renderer.h"
#include "surface.h"

bool Application::init() {
    // -------------------------------
    // Initialize GLFW
    // -------------------------------
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

    // -------------------------------
    // Initialize WebGPU Instance
    // -------------------------------
    wgpu::Instance instance = wgpu::createInstance();

    // -------------------------------
    // Create Surface
    // -------------------------------
    surface = getSurface(window, instance);
    if (!surface) {
        std::cerr << "Failed to create surface\n";
        return false;
    }

    // -------------------------------
    // Request Adapter
    // -------------------------------
    wgpu::Adapter adapter = getAdapter(instance, surface);
    if (!adapter) {
        std::cerr << "Failed to request adapter\n";
        return false;
    }

    // get adapter info
    wgpu::AdapterInfo adapterInfo;
    adapter.getInfo(&adapterInfo);
    std::cout << "Using device: " << adapterInfo.device.data << "\n";

    // -------------------------------
    // Request Device
    // -------------------------------
    device = getDevice(adapter);
    if (!device) {
        std::cerr << "Failed to request device\n";
        return false;
    }

    // Configure surface
    configureSurface(surface, adapter, device);

    return true;
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        auto [surfaceTexture, targetView] = getNextSurfaceViewData(surface);
        if (!targetView) break;

        // Create command encoder
        wgpu::CommandEncoderDescriptor encoderDesc = {};
        encoderDesc.nextInChain = nullptr;
        encoderDesc.label = toStringView("Encoder");

        wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);

        wgpu::RenderPassColorAttachment renderPassColorAttachment = clearColorAttachment(targetView, { 1.0f, 0.0f, 0.0f, 1.0f });


        wgpu::RenderPassDescriptor renderPassDesc = {};
        renderPassDesc.depthStencilAttachment = nullptr;
        renderPassDesc.timestampWrites = nullptr;
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &renderPassColorAttachment;

        // Submit render pass
        wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);
        renderPass.end();

        wgpu::CommandBuffer commands = encoder.finish();
        device.getQueue().submit(1, &commands);

        // Present surface
        surface.present();

        glfwPollEvents();
        device.poll(true, nullptr);
    }
}

void Application::terminate() {
    surface.unconfigure();
    glfwDestroyWindow(window);
    glfwTerminate();
}
