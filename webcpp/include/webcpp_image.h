#ifndef WEBCPP_IMAGE_H
#define WEBCPP_IMAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "webcpp_gpu.h"

// Image format
typedef enum {
    IMAGE_FORMAT_RGBA,
    IMAGE_FORMAT_RGB,
    IMAGE_FORMAT_GRAY,
    IMAGE_FORMAT_GRAY_ALPHA
} image_format_t;

// Image
typedef struct {
    uint32_t width;
    uint32_t height;
    image_format_t format;
    uint8_t* data;
    size_t size;
    gpu_texture_t* texture;
} image_t;

// Image cache
typedef struct {
    image_t** images;
    uint32_t capacity;
    uint32_t count;
} image_cache_t;

// Initialize image system
bool image_init(void);

// Load image from file
image_t* image_load(const char* filename);

// Load image from memory
image_t* image_load_memory(const void* data, size_t size);

// Create image
image_t* image_create(uint32_t width, uint32_t height, image_format_t format);

// Create image cache
image_cache_t* image_create_cache(uint32_t capacity);

// Get image from cache
image_t* image_get_from_cache(image_cache_t* cache, const char* filename);

// Add image to cache
bool image_add_to_cache(image_cache_t* cache, const char* filename, image_t* image);

// Upload image to GPU
bool image_upload_to_gpu(image_t* image);

// Update GPU texture
bool image_update_gpu_texture(image_t* image, uint32_t x, uint32_t y, uint32_t width, uint32_t height);

// Generate mipmaps
bool image_generate_mipmaps(image_t* image);

// Cleanup image
void image_destroy(image_t* image);

// Cleanup image cache
void image_destroy_cache(image_cache_t* cache);

// Shutdown image system
void image_shutdown(void);

#endif // WEBCPP_IMAGE_H 