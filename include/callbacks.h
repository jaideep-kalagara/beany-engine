#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <webgpu/webgpu-raii.hpp>

namespace callbacks {
        namespace device {
                inline void deviceLostCallback(const WGPUDevice*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*) {
                        std::cerr << "Device lost (reason " << reason << "): " << message.data << "\n";
                }

                inline void uncapturedErrorCallback(const WGPUDevice*, WGPUErrorType type, WGPUStringView message, void*, void*) {
                        std::cerr << "Uncaptured error (type " << type << "): " << message.data << "\n";
                }
        }


        // quene
        namespace quene {
                inline void queueWorkDoneCallback(WGPUQueueWorkDoneStatus status, void*, void*) {
                        std::cerr << "Queue work done (status " << status << ")\n";
                }
        }

}

#endif // CALLBACKS_H