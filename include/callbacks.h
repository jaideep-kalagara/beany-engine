#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <webgpu/webgpu-raii.hpp>

void deviceLostCallback(const WGPUDevice*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*) {
        std::cerr << "Device lost (reason " << reason << "): " << message.data << "\n";
}

void uncapturedErrorCallback(const WGPUDevice*, WGPUErrorType type, WGPUStringView message, void*, void*) {
        std::cerr << "Uncaptured error (type " << type << "): " << message.data << "\n";
}

#endif // CALLBACKS_H