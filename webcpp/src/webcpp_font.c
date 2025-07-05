#include "webcpp_font.h"
#include <stdlib.h>
#include <string.h>

// Initialize font system
bool font_init(void) {
    // TODO: Initialize font system
    return true;
}

// Load font
font_t* font_load(const char* family, float size, bool bold, bool italic) {
    font_t* font = (font_t*)malloc(sizeof(font_t));
    if (!font) return NULL;

    // Copy font properties
    font->family = strdup(family);
    font->size = size;
    font->bold = bold;
    font->italic = italic;

    // Initialize metrics
    font->metrics.ascent = 0.0f;
    font->metrics.descent = 0.0f;
    font->metrics.line_height = 0.0f;
    font->metrics.x_height = 0.0f;
    font->metrics.cap_height = 0.0f;

    // Create font atlas
    font->atlas = font_create_atlas(font, 1024, 1024);
    if (!font->atlas) {
        free(font->family);
        free(font);
        return NULL;
    }

    return font;
}

// Create font atlas
font_atlas_t* font_create_atlas(font_t* font, uint32_t width, uint32_t height) {
    if (!font) return NULL;

    font_atlas_t* atlas = (font_atlas_t*)malloc(sizeof(font_atlas_t));
    if (!atlas) return NULL;

    // Initialize atlas properties
    atlas->width = width;
    atlas->height = height;
    atlas->glyph_count = 0;
    atlas->glyph_map = NULL;
    atlas->metrics = NULL;

    // Create texture
    atlas->texture = gpu_create_texture(NULL, width, height, 0, 0, NULL);
    if (!atlas->texture) {
        free(atlas);
        return NULL;
    }

    // TODO: Generate glyph atlas
    // - Load font file
    // - Rasterize glyphs
    // - Pack glyphs into texture
    // - Generate glyph metrics

    return atlas;
}

// Get glyph metrics
bool font_get_glyph_metrics(font_t* font, uint32_t code, glyph_metrics_t* metrics) {
    if (!font || !font->atlas || !metrics) return false;

    // Find glyph in atlas
    uint32_t index = font->atlas->glyph_map[code];
    if (index >= font->atlas->glyph_count) return false;

    // Copy metrics
    *metrics = font->atlas->metrics[index];
    return true;
}

// Create text layout
text_layout_t* font_create_layout(const char* text, font_t* font, float width, float height) {
    if (!text || !font) return NULL;

    text_layout_t* layout = (text_layout_t*)malloc(sizeof(text_layout_t));
    if (!layout) return NULL;

    // Initialize layout properties
    layout->x = 0.0f;
    layout->y = 0.0f;
    layout->width = width;
    layout->height = height;
    layout->line_height = font->metrics.line_height;
    layout->text = strdup(text);
    layout->font = font;
    layout->color = 0xFFFFFFFF;
    layout->alignment = strdup("left");

    return layout;
}

// Measure text
bool font_measure_text(font_t* font, const char* text, float* width, float* height) {
    if (!font || !text || !width || !height) return false;

    float w = 0.0f;
    float h = font->metrics.line_height;
    float line_width = 0.0f;

    // Measure each character
    for (const char* p = text; *p; p++) {
        glyph_metrics_t metrics;
        if (!font_get_glyph_metrics(font, *p, &metrics)) continue;

        line_width += metrics.advance;
        if (*p == '\n') {
            w = (w > line_width) ? w : line_width;
            line_width = 0.0f;
            h += font->metrics.line_height;
        }
    }

    // Handle last line
    w = (w > line_width) ? w : line_width;

    *width = w;
    *height = h;
    return true;
}

// Render text
bool font_render_text(gpu_context_t* ctx, text_layout_t* layout) {
    if (!ctx || !layout || !layout->font || !layout->font->atlas) return false;

    // TODO: Implement text rendering
    // - Set up vertex buffer with text quads
    // - Set up texture coordinates
    // - Set up colors
    // - Draw text using GPU

    return true;
}

// Cleanup font
void font_destroy(font_t* font) {
    if (!font) return;

    if (font->family) {
        free(font->family);
    }

    if (font->atlas) {
        if (font->atlas->texture) {
            gpu_destroy_context(font->atlas->texture);
        }
        if (font->atlas->glyph_map) {
            free(font->atlas->glyph_map);
        }
        if (font->atlas->metrics) {
            free(font->atlas->metrics);
        }
        free(font->atlas);
    }

    free(font);
}

// Cleanup text layout
void font_destroy_layout(text_layout_t* layout) {
    if (!layout) return;

    if (layout->text) {
        free(layout->text);
    }
    if (layout->alignment) {
        free(layout->alignment);
    }

    free(layout);
}

// Shutdown font system
void font_shutdown(void) {
    // TODO: Cleanup font system
} 