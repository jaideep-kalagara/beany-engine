#ifndef APP_H
#define APP_H

#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>

class Application {
public:
    bool init();
    void mainLoop();
    void terminate();

private:
    // GLFW window
    GLFWwindow* window = nullptr;

    // WebGPU resources
    wgpu::RenderPipeline pipeline;
    wgpu::Device device;
    wgpu::Queue queue;
    wgpu::Surface surface;

    // resources
    wgpu::Buffer vertexBuffer;

    // functions
    void initializeBuffers();
};

#endif // APP_H
