#include <fstream>
#include <cassert>
#include <iostream>

#include "hash.hpp"


Hash::Hash(int boardsize)
{
    initialize(boardsize, boardsize);
}

Hash::Hash(int height, int width)
{
    // rectangular boards
    initialize(height, width);
}

Hash::~Hash()
{
    free_buckets();
    delete[] m_hashtable;
    std::free(m_poly_terms);
}

void Hash::initialize(int height, int width)
{
    std::cerr << "initializing hash table\n";
    std::memset(m_hashtable, 0, CAPACITY * sizeof(Bucket));
    m_boardsize[0] = height;
    m_boardsize[1] = width;
    m_num_points = height * width;
    m_poly_terms = (uint64_t*) std::malloc(m_num_points * sizeof(uint64_t));
    m_poly_terms[0] = 1;
    for (int i = 1; i < m_num_points; i++) {
        m_poly_terms[i] = m_poly_terms[i-1] * 3;
    }
}

void Hash::free_buckets()
{
    if (typeid(manager) == typeid(CustomMemoryManager)) {
        return;
    }
    for (uint64_t i = 0; i < CAPACITY; i++) {
        if (m_hashtable[i] != 0) {
            std::free(m_hashtable[i]);
        }
    }
}

void Hash::change_boardsize(int height, int width)
{
    free_buckets();
    std::free(m_poly_terms);
    initialize(height, width);
}

void Hash::clear()
{
    free_buckets();
    std::memset(m_hashtable, 0, CAPACITY * sizeof(Bucket));
}

uint64_t Hash::hash_func(Grid &board2d)
{
    uint64_t hashcode = 0;
    int height = (int) board2d.size();
    int width = (int) board2d[0].size();
    for (int r = height-1; r >= 0; r--) {
        for (int c = 0; c < width; c++) {
            hashcode = hashcode * 3 + board2d[r][c];
        }
    }
    return hashcode;
}

uint64_t Hash::hash_func(uint64_t hashcode, int point, int color)
{
    int canonical_point = GoBoardUtil::point_to_canonical_point(point, m_boardsize);
    int exponent = m_num_points - 1 - canonical_point;
    return hashcode + color * m_poly_terms[exponent];
}

uint64_t Hash::linear_congruence_func(uint64_t hashcode)
{
    return (LCG_A * hashcode) % LCG_M;
}

Entry Hash::format_entry_insert(Entry code, int value)
{
    Entry entry = code;
    entry |= static_cast<Entry>(value) << CODE_BITS;
    return entry;
}

int Hash::format_entry_get(Entry entry)
{
    return static_cast<int>((entry & VALUE_MASK) >> CODE_BITS);
}

bool Hash::insert(uint64_t hashcode, int value)
{
    uint64_t idx = hashcode >> CODE_BITS;
    Entry code = hashcode & CODE_MASK;

    Entry entry = format_entry_insert(code, value);
    if (m_hashtable[idx] != 0) {
        m_hashtable[idx] = BucketUtil::insert(m_hashtable[idx], entry);
    }
    else {
        m_hashtable[idx] = BucketUtil::initialize(entry);
    }
    m_size++;
    return true;
}

int Hash::get(uint64_t hashcode)
{
    uint64_t idx = hashcode >> CODE_BITS;
    Entry code = hashcode & CODE_MASK;

    if (m_hashtable[idx] == 0) {
        return -1;
    }

    std::array<Entry, 2> t = BucketUtil::get(m_hashtable[idx], code);   // (entry, found)
    if (t[1] == 0) {
        return -1;
    }
    
    return format_entry_get(t[0]);
}

bool Hash::set_proof_bit(uint64_t hashcode)
/* Return true if the proof bit is changed (to 1); false, otherwise. */
{
    uint64_t idx = hashcode >> CODE_BITS;
    Entry code = hashcode & CODE_MASK;

    bool change_bit = BucketUtil::set_proof_bit(m_hashtable[idx], code);
    m_proof_size += change_bit;
    return change_bit;
}

bool Hash::get_proof_bit(uint64_t hashcode)
/* Return true if proof bit is 1. Otherwise, return 0 */
{
    uint64_t idx = hashcode >> CODE_BITS;
    Entry code = hashcode & CODE_MASK;

    return BucketUtil::get_proof_bit(m_hashtable[idx], code);
}

Entry Hash::get_raw(uint64_t hashcode)
{
    uint64_t idx = hashcode >> CODE_BITS;
    Entry code = hashcode & CODE_MASK;

    std::array<Entry, 2> t = BucketUtil::get(m_hashtable[idx], code);   // (entry, found)

    return t[0];
}

uint64_t Hash::size()
{
    return m_size;
}

uint64_t Hash::proof_size()
{
    return m_proof_size;
}

void Hash::clear_proof_bit()
{
    Entry mask = -1;
    mask ^= PROOF_MASK;
    for (uint64_t i = 0; i < CAPACITY; i++) {
        if (m_hashtable[i] != 0) {
            Bucket bucket_load = m_hashtable[i] + ENTRY_SIZE;
            uint64_t bucket_size = BucketUtil::read_entry(m_hashtable[i], 0);
            for (uint64_t j = 0; j < bucket_size; j++) {
                Entry entry = BucketUtil::read_entry(bucket_load, j);
                entry &= mask;
                BucketUtil::write_entry(bucket_load, j, entry);
            }
        }
    }
}

std::string Hash::store(std::string file_name, bool proof_only)
{
    std::ofstream f;
    f.open(file_name, std::ios::binary);

    for (uint64_t idx = 0; idx < CAPACITY; idx++) {
        if (m_hashtable[idx] != 0) {
            Bucket bucket_load = m_hashtable[idx] + ENTRY_SIZE;
            uint64_t bucket_size = BucketUtil::read_entry(m_hashtable[idx], 0);
            if (proof_only == false) {
                f.write((const char*)(&idx), sizeof(uint64_t));
                f.write((const char*)(&bucket_size), ENTRY_SIZE);
                f.write((const char*)bucket_load, bucket_size*ENTRY_SIZE);
            }
            else {
                uint64_t proof_count = 0;
                Bucket tmp_bucket = (Bucket) std::malloc(bucket_size*ENTRY_SIZE);
                for (uint64_t i = 0; i < bucket_size; i++) {
                    Entry entry = BucketUtil::read_entry(bucket_load, i);
                    bool proved = (entry & PROOF_MASK) != 0;
                    if (proved == true) {
                        BucketUtil::write_entry(tmp_bucket, proof_count, entry);
                        proof_count++;
                    }
                }
                if (proof_count > 0) {
                    f.write((const char*)(&idx), sizeof(uint64_t));
                    f.write((const char*)(&proof_count), ENTRY_SIZE);
                    f.write((const char*)tmp_bucket, proof_count*ENTRY_SIZE);
                }
                std::free(tmp_bucket);
            }
        }
    }

    f.flush();
    f.close();
    return file_name;
}

std::string Hash::load(std::string file_name)
{
    std::ifstream f;
    f.open(file_name, std::ios::binary);
    if (not f) {
        std::cerr << "Failed to load solution from " << file_name << "\n";
        return "";
    }
    clear();

    f.seekg(0, f.end);
    uint64_t length = f.tellg();
    f.seekg(0, f.beg);

    uint64_t num_byte_read = 0;

    while (num_byte_read < length)
    {
        uint64_t idx = 0, bucket_size = 0;
        f.read((char*)(&idx), sizeof(uint64_t));
        f.read((char*)(&bucket_size), ENTRY_SIZE);
        m_hashtable[idx] = (Bucket) manager.malloc((bucket_size+1)*ENTRY_SIZE);
        BucketUtil::write_entry(m_hashtable[idx], 0, bucket_size);
        Bucket bucket_load = m_hashtable[idx] + ENTRY_SIZE;
        f.read((char*)bucket_load, bucket_size*ENTRY_SIZE);

        m_size += bucket_size;

        num_byte_read += sizeof(uint64_t) + (bucket_size+1)*ENTRY_SIZE;
    }
    assert(num_byte_read == length);

    f.close();
    clear_proof_bit();
    return file_name;
}

/****************************************************************/
/****************************************************************/
/****************************************************************/

Bucket BucketUtil::initialize()
{
    Bucket bucket = (Bucket) manager.malloc(ENTRY_SIZE);
    write_entry(bucket, 0, 0);
    return bucket;
}

Bucket BucketUtil::initialize(Entry entry)
{
    Bucket bucket = (Bucket) manager.malloc(2*ENTRY_SIZE);
    write_entry(bucket, 0, 1);
    write_entry(bucket, 1, entry);
    return bucket;
}

void BucketUtil::write_entry(Bucket bucket, uint64_t idx, Entry entry)
{
    std::memcpy(bucket+idx*ENTRY_SIZE, &entry, ENTRY_SIZE);
}

Entry BucketUtil::read_entry(Bucket bucket, uint64_t idx)
{
    Entry entry = 0;
    std::memcpy(&entry, bucket+idx*ENTRY_SIZE, ENTRY_SIZE);
    return entry;
}

Bucket BucketUtil::insert(Bucket bucket, Entry entry)
{
    Bucket bucket_load = bucket + ENTRY_SIZE;   // the actual array of entries inside bucket
    uint64_t size = read_entry(bucket, 0);            // 0-th encodes the size
    Entry code = entry & CODE_MASK;

    std::array<int, 2> t = binary_search(bucket_load, code, 0, size-1);    // (idx, found)
    int idx = t[0];
    assert(t[1] == 0);

    bucket = (Bucket) manager.realloc(bucket, (size+2)*ENTRY_SIZE, (size+1)*ENTRY_SIZE);  // 0-th + size + one new entry
    assert(bucket);
    bucket_load = bucket + ENTRY_SIZE;

    manager.memmove(bucket_load+(idx+1)*ENTRY_SIZE, bucket_load+idx*ENTRY_SIZE, (size-idx)*ENTRY_SIZE);
    write_entry(bucket_load, idx, entry);

    size += 1;
    write_entry(bucket, 0, size);   // size + 1
    return bucket;
}

std::array<Entry, 2> BucketUtil::get(Bucket bucket, Entry code)
/* Return (entry, found)
 * when size=0, this method won't be called from Hash */
{
    Bucket bucket_load = bucket + ENTRY_SIZE;
    uint64_t size = read_entry(bucket, 0);

    std::array<int, 2> t = binary_search(bucket_load, code, 0, size-1);    // (idx, found)
    Entry entry = read_entry(bucket_load, t[0]*t[1]);
    return {entry, static_cast<Entry>(t[1])};
}

bool BucketUtil::set_proof_bit(Bucket bucket, Entry code)
{
    Bucket bucket_load = bucket + ENTRY_SIZE;
    uint64_t size = read_entry(bucket, 0);

    std::array<int, 2> t = binary_search(bucket_load, code, 0, size-1);    // (idx, found)
    assert(t[1]);
    int idx = t[0] * t[1];
    Entry entry = read_entry(bucket_load, idx);

    bool bit_changed = (entry & PROOF_MASK) == 0;
    entry |= PROOF_MASK;
    write_entry(bucket_load, idx, entry);
    return bit_changed;
}

bool BucketUtil::get_proof_bit(Bucket bucket, Entry code)
{
    Bucket bucket_load = bucket + ENTRY_SIZE;
    uint64_t size = read_entry(bucket, 0);

    std::array<int, 2> t = binary_search(bucket_load, code, 0, size-1);    // (idx, found)
    assert(t[1]);
    Entry entry = read_entry(bucket_load, t[0]);
    bool proved = (entry & PROOF_MASK) != 0;
    return proved;
}

std::array<int, 2> BucketUtil::binary_search(Bucket bucket_load, Entry code, int low, int high)
/* Return <idx, found>.
 * If found==0, idx is the idx to insert.
 * If found==1, idx is the real idx of the entry. */
{
    if (low > high) {
        return {low, 0};    // not inside bucket
    }
    else {
        int mid = (low + high) / 2;
        Entry entry_code = read_entry(bucket_load, mid) & CODE_MASK;
        if (code == entry_code) {
            return {mid, 1};
        }
        else if (code > entry_code) {
            return binary_search(bucket_load, code, mid+1, high);
        }
        else {
            return binary_search(bucket_load, code, low, mid-1);
        }
    }
}
