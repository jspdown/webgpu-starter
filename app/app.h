#ifndef APP_H
#define APP_H

#include <memory>

#include "context.h"

namespace app {

    class App : public FrameHandler {
    public:
        explicit App(std::unique_ptr<Context> ctx);

        void Run();

    private:
        void HandleFrame() override;

    private:
        std::unique_ptr<Context> m_context;
        WGPURenderPipeline m_pipeline;
    };

}
#endif
