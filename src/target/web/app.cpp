#include "app.h"

namespace app {

    WebApp::WebApp(uint32_t width, uint32_t height) :
            App(std::make_unique<WebContext>(width, height)) {}

}
