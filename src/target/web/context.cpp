#include "context.h"

#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>

namespace app {

    WebContext::WebContext(uint32_t width, uint32_t height):
        m_device(emscripten_webgpu_get_device()),
        m_queue(wgpuDeviceGetQueue(m_device)),
        m_preferredTextureFormat(WGPUTextureFormat_BGRA8Unorm) {

        WGPUInstanceDescriptor instanceDesc{};
        WGPUInstance instance = wgpuCreateInstance(&instanceDesc);

        WGPUSurfaceDescriptorFromCanvasHTMLSelector canvasDesc = {};
        canvasDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
        canvasDesc.selector = "canvas";

        WGPUSurfaceDescriptor surfDesc = {};
        surfDesc.nextInChain = &canvasDesc.chain;

        WGPUSurface surface = wgpuInstanceCreateSurface(instance, &surfDesc);

        WGPURequestAdapterOptions adapterOpts{};
        adapterOpts.compatibleSurface = surface;

        WGPUSwapChainDescriptor swapChainDesc = {
                .nextInChain = nullptr,
                .label = "swapchain",
                .usage = WGPUTextureUsage_RenderAttachment,
                .format = m_preferredTextureFormat,
                .width = width,
                .height = height,
                .presentMode = WGPUPresentMode_Fifo,
        };

        m_swapchain = wgpuDeviceCreateSwapChain(m_device, surface, &swapChainDesc);
    }

    WebContext::~WebContext() noexcept {
        wgpuSwapChainRelease(m_swapchain);
        wgpuQueueRelease(m_queue);
        wgpuDeviceRelease(m_device);
    }


    EM_BOOL handleFrame(double, void *data) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto *handler = reinterpret_cast<FrameHandler *>(data);

        handler->HandleFrame();

        return EM_TRUE;
    }

    void WebContext::Loop(FrameHandler *handler) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        auto *data = reinterpret_cast<void *>(handler);

        emscripten_request_animation_frame_loop(handleFrame, data);
    }

    WGPUDevice WebContext::GetDevice() const {
        return m_device;
    }

    WGPUQueue WebContext::GetQueue() const {
        return m_queue;
    }

    WGPUSwapChain  WebContext::GetSwapChain() const {
        return m_swapchain;
    }

    WGPUTextureFormat WebContext::GetPreferredTextureFormat() const {
        return m_preferredTextureFormat;
    }

}
