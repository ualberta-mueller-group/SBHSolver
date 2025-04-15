#ifndef HASH_H
#define HASH_H

#include <array>

#include "configs.hpp"
#include "memory_manager.hpp"
#include "board_util.hpp"


typedef uint64_t            Entry;
typedef unsigned char*      Bucket;


extern MemoryManager manager;


// table of buckets
const uint64_t CAPACITY = (uint64_t) 1 << IDX_BITS;

// LCG parameters
const uint64_t LCG_M = (uint64_t) 1 << (IDX_BITS + CODE_BITS);   // range of all hashcode
const uint64_t LCG_A = (uint64_t) 1037;  // 2^20+1; 1.4M

// entry masks
const Entry CODE_MASK = (Entry) -1 >> (8*sizeof(Entry) - CODE_BITS);
const Entry VALUE_MASK = (Entry) 1 << CODE_BITS;
const Entry PROOF_MASK = (Entry) 1 << (CODE_BITS + 1);


class Hash
{
public:
    Bucket* m_hashtable = new Bucket[CAPACITY]; // call default constructor of Entry
    int m_boardsize[2];
    int m_num_points;
    uint64_t* m_poly_terms;

    //Hash() {};
    Hash(int boardsize=5);
    Hash(int height, int width);
    ~Hash();

    void initialize(int height, int width);

    void free_buckets();

    void change_boardsize(int height, int width);

    void clear();

    uint64_t hash_func(Grid &board2d);

    uint64_t hash_func(uint64_t hashcode, int point, int color);

    uint64_t linear_congruence_func(uint64_t hashcode);

    Entry format_entry_insert(Entry code, int value);

    int format_entry_get(Entry entry);

    bool insert(uint64_t hashcode, int value);

    int get(uint64_t hashcode);

    bool set_proof_bit(uint64_t hashcode);

    bool get_proof_bit(uint64_t hashcode);

    uint64_t size();

    uint64_t proof_size();

    void clear_proof_bit();

    std::string store(std::string file_name, bool proof_only=true);

    std::string load(std::string file_name);

    Entry get_raw(uint64_t idx);

private:
    uint64_t m_size = 0;
    uint64_t m_proof_size = 0;
};

class BucketUtil
{
public:
    static Bucket initialize();

    static Bucket initialize(Entry entry);

    static void write_entry(Bucket bucket, uint64_t idx, Entry entry);

    static Entry read_entry(Bucket bucket, uint64_t idx);

    static Bucket insert(Bucket bucket, Entry entry);

    static std::array<Entry, 2> get(Bucket bucket, Entry code);

    static bool set_proof_bit(Bucket bucket, Entry code);

    static bool get_proof_bit(Bucket bucket, Entry code);

    static std::array<int, 2> binary_search(Bucket bucket_load, Entry code, int low, int high);
};

#endif
