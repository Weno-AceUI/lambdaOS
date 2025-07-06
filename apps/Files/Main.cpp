#include "../../WebCpp/include/webcpp.h"
#include <stdio.h> // For fprintf and printf

int main() {
    webcpp_config_t config = {
        .max_memory = 128 * 1024 * 1024, // 128MB, adjust as needed
        .enable_gpu = true,
        .enable_threading = true,
        .js_engine = "v8"
    };
    if (!webcpp_init(&config)) {
        fprintf(stderr, "Error: Failed to initialize WebCpp for Files app.\n");
        return 1;
    }

    webcpp_context_t* ctx = webcpp_create_context();
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create WebCpp context for Files app.\n");
        webcpp_shutdown();
        return 1;
    }

    // Load the Files.html file for the application's UI
    // This uses the function we previously added to webcpp.h and webcpp.c
    if (!webcpp_load_html_file(ctx, "Files.html")) {
        fprintf(stderr, "Error: Files app failed to load Files.html\n");
        webcpp_destroy_context(ctx);
        webcpp_shutdown();
        return 1;
    }
    printf("Files app: Files.html loaded successfully.\n");

    // Main loop or event handling would go here

    webcpp_destroy_context(ctx);
    webcpp_shutdown();
    return 0;
}