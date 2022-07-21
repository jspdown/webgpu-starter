#ifndef WEBGPU_CONTEXT_H
#define WEBGPU_CONTEXT_H

#include "../../context.h"

namespace app {

    // WebContext is a Context for web targets. This class provides a WebGPU context
    // using emscripten constructs.
    class WebContext : public Context {
    public:
        WebContext(uint32_t width, uint32_t height);
        ~WebContext() noexcept override;

        void Loop(FrameHandler *handler) override;

        [[nodiscard]] WGPUDevice GetDevice() const override;
        [[nodiscard]] WGPUQueue GetQueue() const override;
        [[nodiscard]] WGPUSwapChain GetSwapChain() const override;
        [[nodiscard]] WGPUTextureFormat GetPreferredTextureFormat() const override;

    private:
        WGPUDevice m_device;
        WGPUQueue m_queue;
        WGPUSwapChain m_swapchain;
        WGPUTextureFormat m_preferredTextureFormat;
    };
}

#endif
