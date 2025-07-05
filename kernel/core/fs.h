#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stdbool.h>

#define FS_MAX_FILENAME 64

typedef struct file {
    uint32_t id;
    char name[FS_MAX_FILENAME];
    uint64_t size;
    uint64_t pos;
    void* fs_data;
} file_t;

void fs_init(void);
file_t* fs_open(const char* path, uint32_t flags);
int fs_read(file_t* file, void* buffer, uint64_t size);
int fs_write(file_t* file, const void* buffer, uint64_t size);
void fs_close(file_t* file);

#endif // FS_H 