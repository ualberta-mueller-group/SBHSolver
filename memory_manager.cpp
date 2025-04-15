#include <cstdlib>
#include <iostream>

#include "memory_manager.hpp"


CustomMemoryManager::CustomMemoryManager()
{
    for (int i = 0; i < 8; i++) {
        recycled_list[i] = (unsigned char*) std::malloc(sizeof(unsigned char*));
        *((uint64_t*) recycled_list[i]) = 1;
    }
}

CustomMemoryManager::~CustomMemoryManager()
{
    std::free(pool);
    for (int i = 0; i < 8; i++) {
        std::free(recycled_list[i]);
    }
    std::free(recycled_list);
}

void* CustomMemoryManager::malloc(size_t size)
{
    unsigned char* ptr = get_from_recycled_list(size);
    if (ptr == 0) {
        ptr = pos;
        pos += size;
        check_mem_availability(pos <= end);
    }
    return ptr;
}

void* CustomMemoryManager::realloc(void* ptr, size_t size, size_t old_size)
{
    unsigned char* new_ptr = get_from_recycled_list(size);
    if (new_ptr != 0) {
        memmove(new_ptr, ptr, old_size);
        add_to_recycled_list((unsigned char*)ptr, old_size);
    }
    else {
        if (((unsigned char*)ptr+old_size) == pos) {
            new_ptr = (unsigned char*) ptr;
            pos += size - old_size;
            check_mem_availability(pos <= end);
        }
        else {
            new_ptr = pos;
            pos += size;
            check_mem_availability(pos <= end);
            memmove(new_ptr, ptr, old_size);
            add_to_recycled_list((unsigned char*)ptr, old_size);
        }
    }
    return new_ptr;
}

void CustomMemoryManager::free(void* ptr, size_t size)
{
    add_to_recycled_list((unsigned char*)ptr, size);
    return;
}

void CustomMemoryManager::add_to_recycled_list(unsigned char* ptr, size_t size)
{
    if (size < 8) {
        add_chunk(ptr, size);
    }
    else {
        unsigned char* bin_ptr = recycled_list[size];
        recycled_list[size] = ptr;
        *((unsigned char**) ptr) = bin_ptr;
    }

    return;
}

unsigned char* CustomMemoryManager::get_from_recycled_list(size_t size)
{
    unsigned char* ptr = 0;
    if (size < 8) {
        ptr = get_chunk(size);
    }
    else {
        ptr = recycled_list[size];
        if (ptr != 0) {
            recycled_list[size] = *((unsigned char**) ptr);
        }
    }
    
    return ptr;
}

void CustomMemoryManager::add_chunk(unsigned char* ptr, size_t size)
{
    unsigned char** list = (unsigned char**) recycled_list[size];
    uint64_t list_size = *((uint64_t*) list);
    list = (unsigned char**) std::realloc(list, (list_size+1)*sizeof(unsigned char*));
    list[list_size] = ptr;
    *((uint64_t*) list) += 1;
    recycled_list[size] = (unsigned char*) list;
}

unsigned char* CustomMemoryManager::get_chunk(size_t size)
{
    unsigned char** list = (unsigned char**) recycled_list[size];
    uint64_t list_size = *((uint64_t*) list);
    unsigned char* ptr = 0;
    if (list_size > 1) {
        ptr = list[list_size-1];
        list = (unsigned char**) std::realloc(list, (list_size-1)*sizeof(unsigned char*));
        *((uint64_t*) list) -= 1;
        recycled_list[size] = (unsigned char*) list;
    }
    return ptr;
}

uint64_t CustomMemoryManager::pool_usage()
{
    return static_cast<uint64_t>(pos - pool);
}

void CustomMemoryManager::check_mem_availability(bool expr)
{
    if (expr == false) {
        std::cerr << "Abort: memory pool is full!\n";
        exit(0);
    }
}
