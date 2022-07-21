/// <reference types="emscripten" />

declare module "app" {
    class WebApp {
        constructor(width: number, height: number);
        Run();
    }

    export interface AddModule extends EmscriptenModule {
        WebApp: typeof WebApp,
    }

    export default function createModule(emscriptenArgs: any): AddModule;
}