#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>


#include <iostream>

#include "callbacks.h"
#include "app.h"
#include "helper.h"
#include "renderer.h"
#include "surface.h"
#include "pipeline.h"
#include "shaders.h"

void Application::initializeBuffers() {
    std::vector<float> vertexData = {
        // x0, y0, r, g, b
        -0.5, -0.5, 1.0, 0.0, 0.0,

        // x1, y1
        +0.5, -0.5, 0.0, 1.0, 0.0,

        // x2, y2
        +0.0, +0.5, 0.0, 0.0, 1.0
    };

    size_t vertexCount = static_cast<uint32_t>(vertexData.size() / 5); // 5 floats per vertex

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = vertexData.size() * sizeof(float);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex; // Vertex usage here!
    bufferDesc.mappedAtCreation = false;
    vertexBuffer = device.createBuffer(bufferDesc);

    queue.writeBuffer(vertexBuffer, 0, vertexData.data(), bufferDesc.size);
}

// Initializes the WebGPU application
bool Application::init() {
    // ----------------------------------------
    // Initialize GLFW
    // ----------------------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    // Create a window without an OpenGL context (WebGPU only)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, "WebGPU Beany Renderer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    // ----------------------------------------
    // Create WebGPU Instance
    // ----------------------------------------
    wgpu::Instance instance = wgpu::createInstance();

    // ----------------------------------------
    // Create Surface from the GLFW window
    // ----------------------------------------
    surface = getSurface(window, instance);
    if (!surface) {
        std::cerr << "Failed to create surface\n";
        return false;
    }

    // ----------------------------------------
    // Request a compatible Adapter
    // ----------------------------------------
    wgpu::Adapter adapter = getAdapter(instance, surface);
    if (!adapter) {
        std::cerr << "Failed to request adapter\n";
        return false;
    }

    // Print selected GPU/device name
    wgpu::AdapterInfo adapterInfo;
    adapter.getInfo(&adapterInfo);
    std::cout << "Using device: " << adapterInfo.device.data << "\n";

    // ----------------------------------------
    // Request Logical Device from Adapter
    // ----------------------------------------
    device = getDevice(adapter);
    if (!device) {
        std::cerr << "Failed to request device\n";
        return false;
    }

    // ----------------------------------------
    // Get Command Queue from Device
    // ----------------------------------------
    queue = device.getQueue();

    // ----------------------------------------
    // Set up callback for when submitted work is done
    // ----------------------------------------
    wgpu::QueueWorkDoneCallbackInfo queueWorkDoneCallbackInfo = {};
    queueWorkDoneCallbackInfo.callback = queueWorkDoneCallback;
    queue.onSubmittedWorkDone(queueWorkDoneCallbackInfo);

    // ----------------------------------------
    // Configure Surface (swapchain settings)
    // ----------------------------------------
    configureSurface(surface, adapter, device);

    // ----------------------------------------
    // Create Shader Module (WGSL)
    // ----------------------------------------
    wgpu::ShaderModule triangleShader = createShaderModuleWGSL("assets/shaders/triangle.wgsl", device);

    // ----------------------------------------
    // Get supported surface formats and create Render Pipeline
    // ----------------------------------------
    wgpu::SurfaceCapabilities capabilities;
    surface.getCapabilities(adapter, &capabilities);
    pipeline = createTrianglePipeline(device, capabilities.formats[0], triangleShader);


    // ----------------------------------------
    // Initialize buffers
    // ----------------------------------------
    initializeBuffers();
    
    return true;
}

// Main render loop
void Application::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        // Exit loop if ESC is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // ----------------------------------------
        // Acquire next surface texture to draw into
        // ----------------------------------------
        auto [surfaceTexture, targetView] = getNextSurfaceViewData(surface);
        if (!targetView) break;

        // ----------------------------------------
        // Create Command Encoder
        // ----------------------------------------
        wgpu::CommandEncoderDescriptor encoderDesc = {};
        encoderDesc.label = toStringView("Encoder");
        wgpu::CommandEncoder encoder = device.createCommandEncoder(encoderDesc);


        // ----------------------------------------
        // Define Render Pass (clearing to red)
        // ----------------------------------------
        wgpu::RenderPassColorAttachment renderPassColorAttachment = 
            clearColorAttachment(targetView, { 0.0f, 0.0f, 0.0f, 1.0f });

        wgpu::RenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &renderPassColorAttachment;

        // ----------------------------------------
        // Begin Render Pass
        // ----------------------------------------
        wgpu::RenderPassEncoder renderPass = encoder.beginRenderPass(renderPassDesc);
        renderPass.setPipeline(pipeline);
        renderPass.setVertexBuffer(0, vertexBuffer, 0, vertexBuffer.getSize());
        renderPass.draw(vertexBuffer.getSize() / sizeof(float) / 5, 1, 0, 0);


        renderPass.end();

        // ----------------------------------------
        // Finish encoding and submit command buffer
        // ----------------------------------------
        wgpu::CommandBuffer commands = encoder.finish();
        queue.submit(1, &commands);



        // ----------------------------------------
        // Present the drawn frame
        // ----------------------------------------
        surface.present();

        // Poll events and drive internal WebGPU state machine
        glfwPollEvents();
        device.poll(false, nullptr);
    }
}

// Cleanup on exit
void Application::terminate() {
    surface.unconfigure();           // Clean up surface swapchain
    glfwDestroyWindow(window);       // Destroy window
    glfwTerminate();                 // Terminate GLFW
}
