// Glue wrapper wraps the app WebIDL generated code and includes
// the types needed in the definition.

#include "app.h"

using namespace app;

// Glue code is generated using emscripten's webidl_binder.
#include <glue.cpp>