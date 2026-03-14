#pragma once
#include <vector>
#include <stdlib.h>

struct ArenaBlock {char* start; char* cursor; char* end;};

struct Arena {

    std::vector<ArenaBlock> blockset;
    size_t blockindex;
    size_t basecap;
    size_t total;

    private:
        ArenaBlock new_block(size_t capacity){
            char* start = (char*) malloc(capacity);

            auto block = ArenaBlock{
                start,
                start,
                start + capacity,
            };
            
            total += capacity;
            blockset.push_back(block);
            blockindex = blockset.size();

            return block;
        }

    public:
        static Arena create(size_t base_capacity){
            Arena ar{}; ar.blockindex = 0; ar.basecap = base_capacity;
            
            ar.new_block(base_capacity);
            return ar;
        };

        void* alloc(size_t size, size_t align, size_t new_block_size = 0){
            if (new_block_size == 0) new_block_size = basecap;

            if (blockset[blockindex-1].cursor + size > blockset[blockindex-1].end){
                new_block(new_block_size); 
            };

            auto& curblock = blockset[blockindex - 1];

            void* cursor = curblock.cursor;
            size_t cur = reinterpret_cast<uintptr_t>(cursor);
            size_t aligned = (cur + align - 1) & ~(align - 1);

            curblock.cursor = reinterpret_cast<char*>(aligned + size);
            
            return cursor;
        };

        ~Arena(){
            for (int i = blockindex - 1; i >= 0; i--){
                free(blockset[i].start);
            };
        };
};

template <typename T>
struct ArenaAllocator {
    Arena* arena;

    using value_type = T;

    explicit ArenaAllocator(Arena* a) : arena(a) {}

    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept : arena(other.arena) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(arena->alloc(n * sizeof(T), alignof(T)));
    }
    void deallocate(T* p, std::size_t n) noexcept {}
};