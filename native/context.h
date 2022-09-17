#ifndef NATIVE_CONTEXT_H
#define NATIVE_CONTEXT_H

#include <app/context.h>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include "GLFW/glfw3native.h"

namespace app {

    class NativeContext : public Context {
    public:
        NativeContext(uint32_t width, uint32_t height);
        ~NativeContext() noexcept override;

        void Loop(FrameHandler *handler) override;

        [[nodiscard]] WGPUDevice GetDevice() const override;
        [[nodiscard]] WGPUQueue GetQueue() const override;
        [[nodiscard]] WGPUSwapChain GetSwapChain() const override;
        [[nodiscard]] WGPUTextureFormat GetPreferredTextureFormat() const override;

    private:
        GLFWwindow *m_window;

        WGPUSwapChain m_swapchain;
        WGPUDevice m_device;
        WGPUQueue m_queue;
        WGPUTextureFormat m_preferredTextureFormat;
    };

}

#endif
