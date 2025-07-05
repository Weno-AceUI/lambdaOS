#include "fs.h"
#include <string.h>

#define MAX_FILES 32
#define MAX_FILE_SIZE 1024

typedef struct file_entry {
    file_t file;
    uint8_t data[MAX_FILE_SIZE];
} file_entry_t;

static file_entry_t files[MAX_FILES];
static uint32_t file_count = 0;

void fs_init(void) {
    memset(files, 0, sizeof(files));
    file_count = 0;
}

file_t* fs_open(const char* path, uint32_t flags) {
    if (file_count >= MAX_FILES) {
        return NULL;
    }
    file_entry_t* entry = &files[file_count];
    strncpy(entry->file.name, path, FS_MAX_FILENAME - 1);
    entry->file.id = file_count;
    entry->file.size = 0;
    entry->file.pos = 0;
    file_count++;
    return &entry->file;
}

int fs_read(file_t* file, void* buffer, uint64_t size) {
    if (file->pos >= file->size) {
        return 0;
    }
    uint64_t to_read = (file->pos + size > file->size) ? (file->size - file->pos) : size;
    memcpy(buffer, files[file->id].data + file->pos, to_read);
    file->pos += to_read;
    return to_read;
}

int fs_write(file_t* file, const void* buffer, uint64_t size) {
    if (file->pos + size > MAX_FILE_SIZE) {
        return -1;
    }
    memcpy(files[file->id].data + file->pos, buffer, size);
    file->pos += size;
    if (file->pos > file->size) {
        file->size = file->pos;
    }
    return size;
}

void fs_close(file_t* file) {
    // For now, just a stub
} 