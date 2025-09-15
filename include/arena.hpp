#pragma once
#include <iostream>
#include <array>

struct ArenaBlock {char* start; char* cursor; char* end;};

struct Arena {
    ArenaBlock blocks[12];
    size_t blockindex;
    size_t basecap;

    private:
        ArenaBlock new_block(size_t capacity){
            char* start = (char*) malloc(capacity);

            auto block = ArenaBlock{
                start,
                start,
                start + capacity,
            };
            if (blockindex > 11){
                throw std::runtime_error("Arena block-array exhausted...");
            }
            blocks[blockindex] = block;
            blockindex++;

            return block;
        }

    public:
        static Arena create(size_t base_capacity){
            Arena ar{}; ar.blockindex = 0; ar.basecap = base_capacity;
            
            ar.new_block(base_capacity);
            return ar;
        };

        template <typename T>
        T* insert(const T entity){
            auto& curblock = blocks[blockindex - 1];
            if (curblock.cursor + sizeof(T) > curblock.end){
                const auto& curblock = new_block(basecap);
            };

            T* ptr = new (curblock.cursor) T(entity);
            curblock.cursor+=sizeof(T);
            return ptr;
        }

        ~Arena(){
            for (int i = blockindex - 1; i >= 0; i--){
                free(blocks[i].start);
            };
        };
};