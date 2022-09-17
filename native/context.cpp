#include "context.h"

#include <iostream>
#include <dawn/native/DawnNative.h>
#include <dawn/dawn_proc.h>

namespace app {

    void printGLFWError(int code, const char *message) {
        std::cerr << "GLFW: " << code << " - " << message << std::endl;
    }

    void printDeviceError(WGPUErrorType errorType, const char *message, void *) {
        switch (errorType) {
            case WGPUErrorType_Validation:
                std::cerr << "[ERROR] DEVICE::VALIDATION: " << message << std::endl;
                break;
            case WGPUErrorType_OutOfMemory:
                std::cerr << "[ERROR] DEVICE::OUT_OF_MEMORY: " << message << std::endl;
                break;
            case WGPUErrorType_DeviceLost:
                std::cerr << "[ERROR] DEVICE::LOST: " << message << std::endl;
                break;
            default:
                std::cerr << "[ERROR] DEVICE::UNKNOWN: " << message << std::endl;
                return;
        }
    }

    void printDeviceLost(WGPUDeviceLostReason reason, char const *message, void *) {
        switch (reason) {
            case WGPUDeviceLostReason_Undefined:
                std::cerr << "[VERBOSE] DEVICE::LOST::UNDEFINED: " << message << std::endl;
                break;
            case WGPUDeviceLostReason_Force32:
                std::cerr << "[VERBOSE] DEVICE::LOST::FORCE32: " << message << std::endl;
                break;
            case WGPUDeviceLostReason_Destroyed:
                break;
        }
    }

    void printDeviceLog(WGPULoggingType type, char const *message, void *) {
        switch (type) {
            case WGPULoggingType_Verbose:
                std::cout << "[VERBOSE] DEVICE: " << message << std::endl;
                break;
            case WGPULoggingType_Warning:
                std::cout << "[WARNING] DEVICE: " << message << std::endl;
                break;
            case WGPULoggingType_Error:
                std::cerr << "[ERROR] DEVICE: " << message << std::endl;
                break;
            default:
                std::cout << "[INFO] DEVICE: " << message << std::endl;
                break;
        }
    }

    void requestAdapterCallback(WGPURequestAdapterStatus, WGPUAdapter received, const char *, void *userdata) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        *reinterpret_cast<WGPUAdapter *>(userdata) = received;
    }

    NativeContext::NativeContext(uint32_t width, uint32_t height) {
        // TODO: Use wgpuSurfaceGetPreferredFormat once Dawn implements it.
        m_preferredTextureFormat = WGPUTextureFormat_BGRA8Unorm;

        glfwSetErrorCallback(printGLFWError);

        if (!glfwInit()) {
            throw std::runtime_error("Couldn't init GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(int(width), int(height), "dawn", nullptr, nullptr);
        if (!m_window) {
            glfwTerminate();
            throw std::runtime_error("Couldn't create a GLFW window");
        }

        DawnProcTable procs = dawn::native::GetProcs();
	    dawnProcSetProcs(&procs);

        WGPUInstanceDescriptor instanceDesc{};
        WGPUInstance instance = wgpuCreateInstance(&instanceDesc);

        WGPUSurfaceDescriptorFromXlibWindow nativeSurfaceDesc{};
        nativeSurfaceDesc.chain.sType = WGPUSType_SurfaceDescriptorFromXlibWindow;
        nativeSurfaceDesc.display = glfwGetX11Display();
        nativeSurfaceDesc.window = glfwGetX11Window(m_window);

        WGPUSurfaceDescriptor surfaceDesc{};
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        surfaceDesc.nextInChain = reinterpret_cast<WGPUChainedStruct *>(&nativeSurfaceDesc);

        WGPUSurface surface = wgpuInstanceCreateSurface(instance, &surfaceDesc);

        WGPURequestAdapterOptions adapterOpts{};
        adapterOpts.compatibleSurface = surface;

        WGPUAdapter adapter = nullptr;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        wgpuInstanceRequestAdapter(instance, &adapterOpts, requestAdapterCallback, reinterpret_cast<void *>(&adapter));

        WGPUDeviceDescriptor deviceDesc{};
        m_device = wgpuAdapterCreateDevice(adapter, &deviceDesc);

        wgpuDeviceSetUncapturedErrorCallback(m_device, printDeviceError, nullptr);
        wgpuDeviceSetDeviceLostCallback(m_device, printDeviceLost, nullptr);
        wgpuDeviceSetLoggingCallback(m_device, printDeviceLog, nullptr);

        m_queue = wgpuDeviceGetQueue(m_device);

        WGPUSwapChainDescriptor swapChainDesc = {
                .nextInChain = nullptr,
                .label = "swapchain",
                .usage = WGPUTextureUsage_RenderAttachment,
                .format = m_preferredTextureFormat,
                .width = width,
                .height = height,
                .presentMode = WGPUPresentMode_Fifo,
                .implementation = 0,
        };

        m_swapchain = wgpuDeviceCreateSwapChain(m_device, surface, &swapChainDesc);
    }

    NativeContext::~NativeContext() noexcept {
        wgpuSwapChainRelease(m_swapchain);
        wgpuQueueRelease(m_queue);
        wgpuDeviceRelease(m_device);

        glfwTerminate();
    }

    void NativeContext::Loop(FrameHandler *handler) {
        while (!glfwWindowShouldClose(m_window)) {
            handler->HandleFrame();
            glfwPollEvents();
        }
    }

    WGPUDevice NativeContext::GetDevice() const {
        return m_device;
    }

    WGPUQueue NativeContext::GetQueue() const {
        return m_queue;
    }

    WGPUSwapChain NativeContext::GetSwapChain() const {
        return m_swapchain;
    }

    WGPUTextureFormat NativeContext::GetPreferredTextureFormat() const {
        return m_preferredTextureFormat;
    }

}