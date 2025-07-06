#include "../../WebCpp/include/webcpp.h"
#include <stdio.h> // For fprintf and printf
#include <stdlib.h> // For free()

// Wrapper for webcpp_list_directory_items to be called from JS
// JS will pass (path), C++ returns JSON string or null on error.
// Note: WebCpp's webcpp_register_function mechanism needs to handle
// how arguments are passed and results are returned (e.g., string pointers).
// This is a conceptual wrapper. The actual signature might depend on WebCpp's binding layer.
const char* list_photos_directory_items_js(webcpp_context_t* ctx, const char* path) {
    if (!ctx || !path) return NULL;
    char* result_json = NULL;
    if (webcpp_list_directory_items(ctx, path, &result_json)) {
        // The caller (JS bridge) would be responsible for managing this string's lifetime
        // or copying it. For now, we assume WebCpp handles this.
        return result_json; // JS bridge needs to know to free this later if it's not auto-managed
    }
    return NULL; // Indicate error
}

const char* get_image_data_url_js(webcpp_context_t* ctx, const char* image_path) {
    if (!ctx || !image_path) return NULL;
    char* data_url = NULL;
    if (webcpp_get_image_data_url(ctx, image_path, &data_url)) {
        return data_url;
    }
    return NULL;
}

int main() {
    webcpp_config_t config = {
        .max_memory = 128 * 1024 * 1024, // 128MB, adjust as needed for photos
        .enable_gpu = true,
        .enable_threading = true,
        .js_engine = "v8"
    };
    if (!webcpp_init(&config)) {
        fprintf(stderr, "Error: Failed to initialize WebCpp for Photos app.\n");
        return 1;
    }

    webcpp_context_t* ctx = webcpp_create_context();
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create WebCpp context for Photos app.\n");
        webcpp_shutdown();
        return 1;
    }

    // Load the Photos.html file for the application's UI
    if (!webcpp_load_html_file(ctx, "Photos.html")) {
        fprintf(stderr, "Error: Photos app failed to load Photos.html\n");
        webcpp_destroy_context(ctx);
        webcpp_shutdown();
        return 1;
    }
    printf("Photos app: Photos.html loaded successfully.\n");

    // Register C++ functions to be callable from JavaScript
    // The names "PhotosAPI.listDirectoryItems" and "PhotosAPI.getImageDataUrl" are how JS will call them.
    // The actual binding mechanism depends on how webcpp_register_function is implemented.
    webcpp_register_function(ctx, "PhotosAPI_listDirectoryItems", (void*)list_photos_directory_items_js);
    webcpp_register_function(ctx, "PhotosAPI_getImageDataUrl", (void*)get_image_data_url_js);

    // Main loop or event handling would go here
    // For a real app, this main would not exit immediately but would enter an event loop
    // or wait for WebCpp to signal shutdown.

    webcpp_destroy_context(ctx);
    webcpp_shutdown();
    return 0;
}