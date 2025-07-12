#ifndef RENDERER_H
#define RENDERER_H

#include <webgpu/webgpu-raii.hpp>

// Creates a color attachment that clears the given texture view with the specified color
wgpu::RenderPassColorAttachment clearColorAttachment(wgpu::TextureView targetView, wgpu::Color color) {
    wgpu::RenderPassColorAttachment attachment = {};

    attachment.nextInChain = nullptr;
    attachment.view = targetView;
    attachment.resolveTarget = nullptr;
    attachment.loadOp = wgpu::LoadOp::Clear;
    attachment.storeOp = wgpu::StoreOp::Store;
    attachment.clearValue = color;

    return attachment;
}

#endif // RENDERER_H