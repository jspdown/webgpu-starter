#ifndef WEB_APP_H
#define WEB_APP_H

#include <memory>

#include "../../app.h"
#include "context.h"

namespace app {

    // WebApp specializes the App class and provides a class easier to construct from javascript.
    // This class is aimed at being consumed by the javascript code.
    class WebApp : public App {
    public:
        WebApp(uint32_t width, uint32_t height);
    };

}

#endif
