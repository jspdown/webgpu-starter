// Glue wrapper wraps the app WebIDL generated code and includes
// the types needed in the definition.

#include "app.h"

using namespace app;

// Bind code is generated using emscripten's webidl_binder.
#include <generated/app.bind.cpp>