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
#if defined(_WIN32)
    wgpu::SurfaceSourceWindowsHWND fromWindowHWND = {};
    fromWindowHWND.hwnd = glfwGetWin32Window(window);
    fromWindowHWND.hinstance = GetModuleHandle(NULL);
    fromWindowHWND.chain.next = nullptr;
    fromWindowHWND.chain.sType = wgpu::SType::SurfaceSourceWindowsHWND;

    wgpu::SurfaceDescriptor descriptor = {};
    descriptor.nextInChain = &fromWindowHWND.chain;
    descriptor.label = {"Surface", 7};

    return instance.createSurface(descriptor);

#elif defined(__APPLE__) && defined(__MACH__)
    NSWindow* nsWindow = glfwGetCocoaWindow(window);
    NSView* contentView = [nsWindow contentView];

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    [contentView setLayer:metalLayer];
    [contentView setWantsLayer:YES];

    wgpu::SurfaceDescriptorFromMetalLayer metalDesc = {};
    metalDesc.layer = metalLayer;
    metalDesc.chain.sType = wgpu::SType::SurfaceDescriptorFromMetalLayer;
    metalDesc.chain.next = nullptr;

    wgpu::SurfaceDescriptor descriptor = {};
    descriptor.nextInChain = &metalDesc.chain;
    descriptor.label = {"Surface", 7};

    return instance.createSurface(descriptor);

#elif defined(__linux__)
    wgpu::SurfaceDescriptorFromXlibWindow xlibDesc = {};
    xlibDesc.display = glfwGetX11Display();
    xlibDesc.window = glfwGetX11Window(window);
    xlibDesc.chain.sType = wgpu::SType::SurfaceDescriptorFromXlibWindow;
    xlibDesc.chain.next = nullptr;

    wgpu::SurfaceDescriptor descriptor = {};
    descriptor.nextInChain = &xlibDesc.chain;
    descriptor.label = {"Surface", 7};

    return instance.createSurface(descriptor);
#else
    #error "Platform not supported"
#endif
}
