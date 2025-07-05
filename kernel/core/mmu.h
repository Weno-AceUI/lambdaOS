#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096

void mmu_init(void);
void* mmu_allocate_pages(uint64_t num_pages);
bool mmu_protect_pages(uint64_t addr, uint64_t size, uint32_t protection);
bool mmu_map_pages(uint64_t addr, uint64_t size, uint32_t protection);
bool mmu_unmap_pages(uint64_t addr, uint64_t size);

#endif // MMU_H 