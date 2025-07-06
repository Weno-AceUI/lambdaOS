#include "../../WebCpp/include/webcpp.h"
#include <stdio.h> // For fprintf and printf

int main() {
    webcpp_config_t config = {
        .max_memory = 128 * 1024 * 1024, // Adjust as needed for music library/playback
        .enable_gpu = true,
        .enable_threading = true,
        .js_engine = "v8"
    };
    if (!webcpp_init(&config)) {
        fprintf(stderr, "Error: Failed to initialize WebCpp for Music app.\n");
        return 1;
    }

    webcpp_context_t* ctx = webcpp_create_context();
    if (!ctx) {
        fprintf(stderr, "Error: Failed to create WebCpp context for Music app.\n");
        webcpp_shutdown();
        return 1;
    }

    // Load the Music.html file for the application's UI
    if (!webcpp_load_html_file(ctx, "Music.html")) {
        fprintf(stderr, "Error: Music app failed to load Music.html\n");
        webcpp_destroy_context(ctx);
        webcpp_shutdown();
        return 1;
    }
    printf("Music app: Music.html loaded successfully.\n");

    // TODO: Register WebCpp functions for music playback control, library access, etc.
    // e.g., webcpp_register_function(ctx, "MusicAPI_play", (void*)play_music_js);

    // Main loop or event handling would go here

    webcpp_destroy_context(ctx);
    webcpp_shutdown();
    return 0;
}