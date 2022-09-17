#include "app.h"

#include <webgpu/webgpu.h>
#include <iostream>

namespace app {

    // createShaderModule creates a new WebGPU shader module from a given WGPU source code.
    WGPUShaderModule createShaderModule(WGPUDevice device, std::string const &source, std::string const &name) {
        WGPUShaderModuleWGSLDescriptor wgslDesc = {
                .chain = WGPUChainedStruct{
                        .next = nullptr,
                        .sType = WGPUSType_ShaderModuleWGSLDescriptor,
                },
                .code = source.c_str(),
        };
        WGPUShaderModuleDescriptor desc = {
                .nextInChain = &wgslDesc.chain,
                .label = name.c_str(),
        };

        return wgpuDeviceCreateShaderModule(device, &desc);
    }

    App::App(std::unique_ptr<Context> context): m_context(std::move(context)) {
        auto vertexShaderModule = createShaderModule(m_context->GetDevice(), R"(
        @vertex fn main(
            @builtin(vertex_index) VertexIndex : u32
        ) -> @builtin(position) vec4<f32> {
            var pos = array<vec2<f32>, 3>(
                vec2<f32>( 0.0,  0.5),
                vec2<f32>(-0.5, -0.5),
                vec2<f32>( 0.5, -0.5)
            );
            return vec4<f32>(pos[VertexIndex], 0.0, 1.0);
        })", "vertex-shader");


        auto fragmentShaderModule = createShaderModule(m_context->GetDevice(), R"(
        @fragment fn main() -> @location(0) vec4<f32> {
            return vec4<f32>(1.0, 0.0, 0.0, 1.0);
        })", "fragment-shader");

        WGPUBlendState blend = {
                .color = WGPUBlendComponent{
                        .operation = WGPUBlendOperation_Add,
                        .srcFactor = WGPUBlendFactor_One,
                        .dstFactor = WGPUBlendFactor_One,
                },
                .alpha = WGPUBlendComponent{
                        .operation = WGPUBlendOperation_Add,
                        .srcFactor = WGPUBlendFactor_One,
                        .dstFactor = WGPUBlendFactor_One,
                },
        };
        WGPUColorTargetState colorTarget{};
        colorTarget.format = m_context->GetPreferredTextureFormat();
        colorTarget.blend = &blend;
        colorTarget.writeMask = WGPUColorWriteMask_All;

        WGPUFragmentState fragment{};
        fragment.module = fragmentShaderModule;
        fragment.entryPoint = "main";
        fragment.targetCount = 1;
        fragment.targets = &colorTarget;

        WGPUVertexState vertex{};
        vertex.module = vertexShaderModule;
        vertex.entryPoint = "main";

        const uint32_t whiteColor = 0xFFFFFFFF;

        WGPUPipelineLayoutDescriptor pipelineLayoutDesc{};
        WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(m_context->GetDevice(), &pipelineLayoutDesc);

        WGPUPrimitiveState primitiveState{};
        primitiveState.topology = WGPUPrimitiveTopology_TriangleList;
        primitiveState.stripIndexFormat = WGPUIndexFormat_Undefined;
        primitiveState.frontFace = WGPUFrontFace_CCW;
        primitiveState.cullMode = WGPUCullMode_None;

        WGPUMultisampleState multisampleState{};
        multisampleState.count = 1;
        multisampleState.mask = whiteColor;

        WGPURenderPipelineDescriptor pipelineDesc{};
        pipelineDesc.label = "render-pipeline";
        pipelineDesc.layout = pipelineLayout;
        pipelineDesc.vertex = vertex;
        pipelineDesc.fragment = &fragment;
        pipelineDesc.primitive = primitiveState;
        pipelineDesc.multisample = multisampleState;

        m_pipeline = wgpuDeviceCreateRenderPipeline(m_context->GetDevice(), &pipelineDesc);
    }

    void App::Run() {
        m_context->Loop(this);
    }

    void App::HandleFrame() {
        WGPUTextureView backBufferView = wgpuSwapChainGetCurrentTextureView(m_context->GetSwapChain());

        WGPURenderPassColorAttachment colorAttachment{};
        colorAttachment.view = backBufferView;
        colorAttachment.loadOp = WGPULoadOp_Clear;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = {0.0f, 0.0f, 0.0f, 0.0f};

        WGPURenderPassDescriptor renderPassInfo{};
        renderPassInfo.label = "render-pass";
        renderPassInfo.colorAttachmentCount = 1;
        renderPassInfo.colorAttachments = &colorAttachment;

        auto encoder = wgpuDeviceCreateCommandEncoder(m_context->GetDevice(), nullptr);
        auto pass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassInfo);

        wgpuRenderPassEncoderSetPipeline(pass, m_pipeline);
        wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
        wgpuRenderPassEncoderEnd(pass);

        auto commands = wgpuCommandEncoderFinish(encoder, nullptr);

        wgpuQueueSubmit(m_context->GetQueue(), 1, &commands);
        wgpuSwapChainPresent(m_context->GetSwapChain());
    }

}