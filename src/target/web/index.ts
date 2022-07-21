import createModule from "app"

async function main() {
    const res = await fetch('app.wasm')
    const wasm = await res.arrayBuffer()

    const adapter = await navigator.gpu.requestAdapter()
    const device = await adapter.requestDevice()

    const module = await createModule({
        wasm,
        preinitializedWebGPUDevice: device,
    })

    const app = await new module.WebApp(500, 500);

    app.Run();
}

main().catch(err => console.error(err))