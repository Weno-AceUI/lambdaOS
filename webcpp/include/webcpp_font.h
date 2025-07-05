#ifndef WEBCPP_FONT_H
#define WEBCPP_FONT_H

#include <stdint.h>
#include <stdbool.h>
#include "webcpp_gpu.h"

// Font metrics
typedef struct {
    float ascent;
    float descent;
    float line_height;
    float x_height;
    float cap_height;
} font_metrics_t;

// Glyph metrics
typedef struct {
    float width;
    float height;
    float bearing_x;
    float bearing_y;
    float advance;
    float u0, v0;  // Texture coordinates
    float u1, v1;  // Texture coordinates
} glyph_metrics_t;

// Font atlas
typedef struct {
    gpu_texture_t* texture;
    uint32_t width;
    uint32_t height;
    uint32_t* glyph_map;  // Maps character codes to glyph indices
    glyph_metrics_t* metrics;
    uint32_t glyph_count;
} font_atlas_t;

// Font
typedef struct {
    char* family;
    float size;
    bool bold;
    bool italic;
    font_metrics_t metrics;
    font_atlas_t* atlas;
} font_t;

// Text layout
typedef struct {
    float x, y;
    float width, height;
    float line_height;
    char* text;
    font_t* font;
    uint32_t color;
    char* alignment;
} text_layout_t;

// Initialize font system
bool font_init(void);

// Load font
font_t* font_load(const char* family, float size, bool bold, bool italic);

// Create font atlas
font_atlas_t* font_create_atlas(font_t* font, uint32_t width, uint32_t height);

// Get glyph metrics
bool font_get_glyph_metrics(font_t* font, uint32_t code, glyph_metrics_t* metrics);

// Create text layout
text_layout_t* font_create_layout(const char* text, font_t* font, float width, float height);

// Measure text
bool font_measure_text(font_t* font, const char* text, float* width, float* height);

// Render text
bool font_render_text(gpu_context_t* ctx, text_layout_t* layout);

// Cleanup font
void font_destroy(font_t* font);

// Cleanup text layout
void font_destroy_layout(text_layout_t* layout);

// Shutdown font system
void font_shutdown(void);

#endif // WEBCPP_FONT_H 