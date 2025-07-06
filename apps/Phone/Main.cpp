#include "../../WebCpp/include/webcpp.h"
#include <stdio.h> // For fprintf and printf

int main() {
    webcpp_config_t config = {
        .max_memory = 128 * 1024 * 1024, // 128MB
        .enable_gpu = true,
        .enable_threading = true,
        .js_engine = "v8"
    };
    if (!webcpp_init(&config)) {
        fprintf(stderr, "Error: Failed to initialize WebCpp for Phone app.\n");
        return 1;
    }

    webcpp_context_t* ctx = webcpp_create_context();
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create WebCpp context for Phone app.\n");
        webcpp_shutdown();
        return 1;
    }

    // Register widget functions (TabBar, Tab, etc.)
    if (!webcpp_register_widget_functions(ctx)) {
        fprintf(stderr, "Error: Failed to register widget functions for Phone app.\n");
        webcpp_destroy_context(ctx);
        webcpp_shutdown();
        return 1;
    }

    // Load the Phone.html file for the application's UI
    if (!webcpp_load_html_file(ctx, "Phone.html")) {
        fprintf(stderr, "Error: Phone app failed to load Phone.html\n");
        webcpp_destroy_context(ctx);
        webcpp_shutdown();
        return 1;
    }
    printf("Phone app: Phone.html loaded successfully.\n");

    // Main loop or event handling would go here
    // For a real app, this main would not exit immediately but would enter an event loop
    // or wait for WebCpp to signal shutdown.

    webcpp_destroy_context(ctx);
    webcpp_shutdown();
    return 0;
}