#ifndef H_MEMORY_MANAGER
#define H_MEMORY_MANAGER

#include <cstdint>
#include <cstring>


const uint64_t ALLOC_SIZE = (uint64_t) 32 << 30;    // large pool of RAM in bytes
const uint64_t RECYCLE_SIZE = 1025;


class BaseMemoryManager
{
public:
    BaseMemoryManager() {};
    ~BaseMemoryManager() {};

    void* malloc(size_t size) { return 0; };

    void* realloc(void* ptr, size_t size, size_t old_size) { return 0; };

    void* memmove(void* dst, void* src, size_t size) { return std::memmove(dst, src, size); };

    void free(void* ptr, size_t size) {};

    uint64_t pool_usage() { return 0; };
};


class DefaultMemoryManager: public BaseMemoryManager
{
public:
    DefaultMemoryManager() {};
    ~DefaultMemoryManager() {};

    void* malloc(size_t size) { return std::malloc(size); };

    void* realloc(void* ptr, size_t size, size_t old_size=0) { return std::realloc(ptr, size); };

    void free(void* ptr, size_t size=0) { std::free(ptr); };
};


class CustomMemoryManager : public BaseMemoryManager
{
public:
    unsigned char* pool = (unsigned char*) std::calloc(ALLOC_SIZE, sizeof(unsigned char));
    unsigned char* pos = pool;                  // pointer to the first unused memory in the pool
    unsigned char* end = pool + ALLOC_SIZE;     // pointer to the first unallocated memory after the pool
    unsigned char** recycled_list = (unsigned char**) std::calloc(RECYCLE_SIZE, sizeof(unsigned char*));

    CustomMemoryManager();
    ~CustomMemoryManager();

    void* malloc(size_t size);

    void* realloc(void* ptr, size_t size, size_t old_size);

    void free(void* ptr, size_t size);

    uint64_t pool_usage();

private:
    void add_to_recycled_list(unsigned char* ptr, size_t size);

    unsigned char* get_from_recycled_list(size_t size);

    /* Handle special cases of chunk of size less than 8 bytes for recycling */
    void add_chunk(unsigned char* ptr, size_t size);

    /* Handle speical cases of chunk of size less than 8 bytes for recycling */
    unsigned char* get_chunk(size_t size);

    void check_mem_availability(bool expr);
};

#endif
