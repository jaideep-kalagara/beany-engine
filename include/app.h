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
    wgpu::Device device;
    wgpu::Surface surface;
};

#endif // APP_H
