#ifndef WEBCPP_RENDER_H
#define WEBCPP_RENDER_H

#include <stdint.h>
#include <stdbool.h>
#include "webcpp_dom.h"

// Render context
typedef struct {
    void* gpu_context;      // GPU context
    void* surface;          // Rendering surface
    uint32_t width;         // Surface width
    uint32_t height;        // Surface height
    float scale;            // Display scale factor
} render_context_t;

// Style properties
typedef struct {
    float x, y;             // Position
    float width, height;    // Size
    float margin[4];        // Margins (top, right, bottom, left)
    float padding[4];       // Padding (top, right, bottom, left)
    float border[4];        // Border width (top, right, bottom, left)
    uint32_t color;         // Text color
    uint32_t background;    // Background color
    float opacity;          // Opacity
    bool visible;           // Visibility
    bool display;           // Display mode
    char* font_family;      // Font family
    float font_size;        // Font size
    char* text_align;       // Text alignment
} style_properties_t;

// Initialize renderer
bool render_init(void);

// Create render context
render_context_t* render_create_context(uint32_t width, uint32_t height);

// Set render target
bool render_set_target(render_context_t* ctx, void* surface);

// Render DOM tree
bool render_dom(render_context_t* ctx, dom_document_t* doc);

// Compute layout
bool render_compute_layout(dom_node_t* node, style_properties_t* style);

// Apply styles
bool render_apply_styles(dom_node_t* node, style_properties_t* style);

// Draw element
bool render_draw_element(render_context_t* ctx, dom_node_t* element, style_properties_t* style);

// Draw text
bool render_draw_text(render_context_t* ctx, dom_node_t* text_node, style_properties_t* style);

// Draw image
bool render_draw_image(render_context_t* ctx, dom_node_t* image_node, style_properties_t* style);

// Clear render target
bool render_clear(render_context_t* ctx, uint32_t color);

// Present rendered content
bool render_present(render_context_t* ctx);

// Cleanup render context
void render_destroy_context(render_context_t* ctx);

// Shutdown renderer
void render_shutdown(void);

#endif // WEBCPP_RENDER_H 