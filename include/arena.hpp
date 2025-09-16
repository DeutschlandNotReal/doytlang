#pragma once
#include <iostream>
#include <array>
#include <string>

struct ArenaBlock {char* start; char* cursor; char* end;};
using namespace std;

struct Arena {
    ArenaBlock blocks[32];
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
            if (blockindex > 31){
                throw std::runtime_error("Arena block-array exhausted...");
            }
            
            total += capacity;
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

        void* alloc(size_t size, size_t new_block_size = 0){
            if (new_block_size == 0) new_block_size = basecap;

            if (blocks[blockindex-1].cursor + size > blocks[blockindex-1].end){
                new_block(new_block_size); 
            };

            auto& curblock = blocks[blockindex - 1];

            void* cursor = curblock.cursor;
            curblock.cursor += size;
            
            return cursor;
        };

        ~Arena(){
            for (int i = blockindex - 1; i >= 0; i--){
                free(blocks[i].start);
            };
        };
};