#include <iostream>
#define WEBGPU_CPP_IMPLEMENTATION 
#include "app.h"



int main() {
    Application app;

    if (!app.init()) {
        return 1;
    }

    app.mainLoop();

    app.terminate();

    return 0;
}