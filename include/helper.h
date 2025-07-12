#ifndef HELPER_H
#define HELPER_H

#include <webgpu/webgpu-raii.hpp>
#include <string>

#include "callbacks.h"

wgpu::Limits getRequiredLimits(wgpu::Adapter& adapter) {
    wgpu::Limits supportedLimits = {};
    adapter.getLimits(&supportedLimits);
    
    wgpu::Limits requiredLimits = wgpu::Default;

    requiredLimits.maxVertexAttributes = 2;
    requiredLimits.maxVertexBuffers = 1;
    // requiredLimits.maxBufferSize = 6 * 2 * sizeof(float);
    requiredLimits.maxVertexBufferArrayStride = 5 * sizeof(float);

    requiredLimits.minUniformBufferOffsetAlignment = supportedLimits.minUniformBufferOffsetAlignment;
    requiredLimits.minStorageBufferOffsetAlignment = supportedLimits.minStorageBufferOffsetAlignment;

    return requiredLimits;
}

// Converts a std::string to wgpu::StringView
inline wgpu::StringView toStringView(const std::string& str) {
    return wgpu::StringView(str.data());
}

// get adapter
wgpu::Adapter getAdapter(wgpu::Instance& instance, wgpu::Surface& surface) {
    wgpu::RequestAdapterOptions adapterOpts;
    adapterOpts.nextInChain = nullptr;
    adapterOpts.powerPreference = wgpu::PowerPreference::HighPerformance;
    adapterOpts.compatibleSurface = surface;

    wgpu::Adapter adapter = instance.requestAdapter(adapterOpts);
    if (!adapter) {
        std::cerr << "Failed to request adapter\n";
        return nullptr;
    }

    return adapter;
}

// get device
wgpu::Device getDevice(wgpu::Adapter& adapter) {
    wgpu::DeviceDescriptor deviceDesc;
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = toStringView("Device");

    deviceDesc.deviceLostCallbackInfo.callback = deviceLostCallback;
    deviceDesc.uncapturedErrorCallbackInfo.callback = uncapturedErrorCallback;

    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = toStringView("Default Queue");

    wgpu::Limits requiredLimits = getRequiredLimits(adapter);
    deviceDesc.requiredLimits = &requiredLimits;

    wgpu::Device device = adapter.requestDevice(deviceDesc);
    if (!device) {
        std::cerr << "Failed to request device\n";
        return nullptr;
    }

    return device;
}

// Configures the WebGPU surface with the provided adapter and device
inline void configureSurface(wgpu::Surface& surface, wgpu::Adapter& adapter, wgpu::Device& device) {
    wgpu::SurfaceConfiguration config = {};
    config.device = device;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.presentMode = wgpu::PresentMode::Fifo;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;
    config.viewFormatCount = 0;
    config.viewFormats = nullptr;
    config.width = 800;
    config.height = 600;

    wgpu::SurfaceCapabilities capabilities;
    surface.getCapabilities(adapter, &capabilities);

    config.format = capabilities.formats[0]; // Choose the first supported format

    surface.configure(config);
}

// Get the next surface view data (frame)
inline std::pair<wgpu::SurfaceTexture, wgpu::TextureView> getNextSurfaceViewData(wgpu::Surface& surface) {
    wgpu::SurfaceTexture surfaceTexture;

    // Get current surface texture
    surface.getCurrentTexture(&surfaceTexture);
    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessOptimal &&
        surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::SuccessSuboptimal) {
        std::cerr << "Failed to get current surface texture\n";
        return { surfaceTexture, nullptr };
    }

    // Create texture view
    wgpu::TextureViewDescriptor viewDescriptor = {};
    viewDescriptor.nextInChain = nullptr;
    viewDescriptor.label = toStringView("Surface texture view");
    viewDescriptor.format = static_cast<wgpu::Texture>(surfaceTexture.texture).getFormat();
    viewDescriptor.dimension = wgpu::TextureViewDimension::_2D;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;
    viewDescriptor.aspect = wgpu::TextureAspect::All;

    wgpu::TextureView targetView = static_cast<wgpu::Texture>(surfaceTexture.texture).createView(viewDescriptor);

    return { surfaceTexture, targetView };
}




#endif // HELPER_H
