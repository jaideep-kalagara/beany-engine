#include <webgpu/webgpu-raii.hpp>
#include <GLFW/glfw3.h>

// Get exposed native window handles from GLFW
#if defined(_WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__) && defined(__MACH__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
    #define GLFW_EXPOSE_NATIVE_X11
#else
    #error "Platform not supported"
#endif

#include <GLFW/glfw3native.h>

#if defined(__APPLE__)
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

wgpu::Surface getSurface(GLFWwindow* window, wgpu::Instance& instance) {
    wgpu::SurfaceDescriptor descriptor = {};
    descriptor.label = {"Surface", 7};

#if defined(_WIN32)
    wgpu::SurfaceSourceWindowsHWND source = {};
    source.hwnd = glfwGetWin32Window(window);
    source.hinstance = GetModuleHandle(nullptr);
    source.chain.sType = wgpu::SType::SurfaceSourceWindowsHWND;
    source.chain.next = nullptr;

    descriptor.nextInChain = &source.chain;

#elif defined(__APPLE__) && defined(__MACH__)
    NSWindow* nsWindow = glfwGetCocoaWindow(window);
    NSView* contentView = [nsWindow contentView];

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    [contentView setLayer:metalLayer];
    [contentView setWantsLayer:YES];

    wgpu::SurfaceSourceMetalLayer source = {};
    source.layer = metalLayer;
    source.chain.sType = wgpu::SType::SurfaceSourceMetalLayer;
    source.chain.next = nullptr;

    descriptor.nextInChain = &source.chain;

#elif defined(__linux__)
    wgpu::SurfaceSourceXlibWindow source = {};
    source.display = glfwGetX11Display();
    source.window = glfwGetX11Window(window);
    source.chain.sType = wgpu::SType::SurfaceSourceXlibWindow;
    source.chain.next = nullptr;

    descriptor.nextInChain = &source.chain;

#else
    #error "Platform not supported"
#endif

    return instance.createSurface(descriptor);
}
