#ifndef CONTEXT_H
#define CONTEXT_H

#include <memory>
#include <functional>
#include <webgpu/webgpu.h>

namespace app {

    class FrameHandler {
    public:
        virtual ~FrameHandler() = default;

        virtual void HandleFrame() = 0;
    };

    class Context {
    public:
        virtual ~Context() = default;

        virtual void Loop(FrameHandler *handler) = 0;

        [[nodiscard]] virtual WGPUDevice GetDevice() const = 0;
        [[nodiscard]] virtual WGPUQueue GetQueue() const = 0;
        [[nodiscard]] virtual WGPUSwapChain GetSwapChain() const = 0;
        [[nodiscard]] virtual WGPUTextureFormat GetPreferredTextureFormat() const = 0;
    };

}

#endif