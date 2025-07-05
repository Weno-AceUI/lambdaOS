#ifndef WEBCPP_GPU_H
#define WEBCPP_GPU_H

#include <stdint.h>
#include <stdbool.h>

// Shader types
typedef enum {
    SHADER_VERTEX,
    SHADER_FRAGMENT,
    SHADER_COMPUTE
} shader_type_t;

// Shader program
typedef struct {
    uint32_t program_id;
    uint32_t vertex_shader;
    uint32_t fragment_shader;
    bool is_linked;
} shader_program_t;

// GPU buffer types
typedef enum {
    BUFFER_VERTEX,
    BUFFER_INDEX,
    BUFFER_UNIFORM,
    BUFFER_STORAGE
} buffer_type_t;

// GPU buffer
typedef struct {
    uint32_t buffer_id;
    buffer_type_t type;
    size_t size;
    void* data;
    bool is_mapped;
} gpu_buffer_t;

// GPU texture
typedef struct {
    uint32_t texture_id;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint32_t internal_format;
    bool is_mipmapped;
} gpu_texture_t;

// GPU context
typedef struct {
    void* native_context;
    shader_program_t* default_program;
    gpu_buffer_t* vertex_buffer;
    gpu_buffer_t* index_buffer;
    gpu_buffer_t* uniform_buffer;
    bool is_initialized;
} gpu_context_t;

// Initialize GPU context
bool gpu_init(void);

// Create GPU context
gpu_context_t* gpu_create_context(void);

// Create shader program
shader_program_t* gpu_create_shader_program(const char* vertex_src, const char* fragment_src);

// Use shader program
bool gpu_use_program(gpu_context_t* ctx, shader_program_t* program);

// Create GPU buffer
gpu_buffer_t* gpu_create_buffer(gpu_context_t* ctx, buffer_type_t type, size_t size, const void* data);

// Update GPU buffer
bool gpu_update_buffer(gpu_buffer_t* buffer, size_t offset, size_t size, const void* data);

// Map GPU buffer
void* gpu_map_buffer(gpu_buffer_t* buffer);

// Unmap GPU buffer
bool gpu_unmap_buffer(gpu_buffer_t* buffer);

// Create GPU texture
gpu_texture_t* gpu_create_texture(gpu_context_t* ctx, uint32_t width, uint32_t height, 
                                 uint32_t format, uint32_t internal_format, const void* data);

// Update GPU texture
bool gpu_update_texture(gpu_texture_t* texture, uint32_t x, uint32_t y, 
                       uint32_t width, uint32_t height, const void* data);

// Generate mipmaps
bool gpu_generate_mipmaps(gpu_texture_t* texture);

// Set texture parameters
bool gpu_set_texture_params(gpu_texture_t* texture, uint32_t param, uint32_t value);

// Bind texture
bool gpu_bind_texture(gpu_context_t* ctx, gpu_texture_t* texture, uint32_t unit);

// Draw primitives
bool gpu_draw(gpu_context_t* ctx, uint32_t mode, uint32_t first, uint32_t count);

// Draw indexed primitives
bool gpu_draw_indexed(gpu_context_t* ctx, uint32_t mode, uint32_t count, uint32_t type, const void* indices);

// Set viewport
bool gpu_set_viewport(gpu_context_t* ctx, int32_t x, int32_t y, uint32_t width, uint32_t height);

// Set clear color
bool gpu_set_clear_color(gpu_context_t* ctx, float r, float g, float b, float a);

// Clear buffers
bool gpu_clear(gpu_context_t* ctx, uint32_t buffers);

// Present frame
bool gpu_present(gpu_context_t* ctx);

// Cleanup GPU context
void gpu_destroy_context(gpu_context_t* ctx);

// Shutdown GPU system
void gpu_shutdown(void);

#endif // WEBCPP_GPU_H 