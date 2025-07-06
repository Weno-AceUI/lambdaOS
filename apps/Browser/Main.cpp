#include "../../WebCpp/include/webcpp.h"
#include <stdio.h> // For fprintf and printf

// Wrapper for webcpp_load_url to be called from JS
bool load_url_js(webcpp_context_t* ctx, const char* url) {
    if (!ctx || !url) {
        fprintf(stderr, "BrowserApp: Invalid arguments to load_url_js.\n");
        return false;
    }
    printf("BrowserApp: Attempting to load URL: %s\n", url);
    // This relies on the actual implementation of webcpp_load_url in webcpp.c
    return webcpp_load_url(ctx, url); 
}

int main() {
    webcpp_config_t config = {
        .max_memory = 256 * 1024 * 1024, // Browsers can be memory intensive
        .enable_gpu = true,
        .enable_threading = true,
        .js_engine = "v8" // Or your chosen engine
    };
    if (!webcpp_init(&config)) {
        fprintf(stderr, "Error: Failed to initialize WebCpp for Browser app.\n");
        return 1;
    }

    webcpp_context_t* ctx = webcpp_create_context();
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create WebCpp context for Browser app.\n");
        webcpp_shutdown();
        return 1;
    }

    // Load the initial Browser UI (address bar, etc.)
    if (!webcpp_load_html_file(ctx, "Browser.html")) {
        fprintf(stderr, "Error: Browser app failed to load Browser.html\n");
        webcpp_destroy_context(ctx);
        webcpp_shutdown();
        return 1;
    }
    printf("Browser app: Browser.html UI loaded successfully.\n");

    webcpp_register_function(ctx, "BrowserAPI_loadUrl", (void*)load_url_js);

    // Main loop or event handling would go here. For a browser, this needs to be persistent.

    webcpp_destroy_context(ctx); // This would typically happen on app close, not immediately.
    webcpp_shutdown();
    return 0;
}