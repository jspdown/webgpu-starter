#include "./context.h"
#include "../../app.h"
#include <iostream>

int main() {
    const uint32_t width = 500;
    const uint32_t height = 500;

    auto context = std::make_unique<app::NativeContext>(width, height);

    app::App application(std::move(context));

    application.Run();
}