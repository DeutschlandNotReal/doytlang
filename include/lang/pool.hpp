#pragma once
#include <utility>

#define DEFAULT_POOL_CAPACITY 1024 
// matched to 1 KiB for RawPool

class RawPool {
    struct PoolBlock { 
        char* buf = nullptr; 
        unsigned int cur = 0, len = 0;

        PoolBlock() = default;
        PoolBlock(const unsigned int B): buf(new char[B]), len(B) {}
        unsigned int available() const { return len - cur; }
    };

    PoolBlock* blocks;
    short count = 1, capacity = 1;

    // gets the top pointer, may create new block if needed
    template <typename T> T* _top() {
        if (sizeof(T) > blocks[count-1].available()) {
            PoolBlock* old_blocks = blocks;
            capacity <<= 1;
            blocks = new PoolBlock[capacity];
            for (int i = 0; i < count; i++) blocks[i] = old_blocks[i];
            blocks[count++] = PoolBlock(DEFAULT_POOL_CAPACITY);
            delete[] old_blocks;
        }
        PoolBlock& top = blocks[count-1];
        T* ptr = (T*)(top.buf + top.cur);
        top.cur += sizeof(T);
        return ptr;
    }

    public:
        PoolBlock operator[](short i) {
            return blocks[i];
        }

        short block_count() const noexcept { return count; }

        RawPool(): blocks(new PoolBlock[1]) {
            blocks[0] = PoolBlock(DEFAULT_POOL_CAPACITY);
        }

        ~RawPool() {
            for (int i = 0; i < count; i++) delete[] blocks[i].buf;
            delete[] blocks;
        }

        template <typename T, typename... args> T* emplace(args&&... params) {
            return new (_top<T>()) T(std::forward<args>(params)...);
        }

        template <typename T> T* append(const T& item) {
            return new (_top<T>()) T(item);
        }
};

template <typename T> class Pool {
    struct PoolBlock { 
        T* buf = nullptr; 
        unsigned int cur = 0, len = 0;
        
        PoolBlock() = default;
        PoolBlock(const unsigned int size): buf(new T[size]), len(size) {}
        unsigned int available() const noexcept { return len - cur; }
    };

    class PoolIterator {
        // only use if done with making pool
        PoolBlock* block; short block_index, block_count;
        int index, gap;
        
        public:
            T peek() const noexcept { return block->buf[index]; }
            T consume() noexcept {
              T current = block->buf[index++];
              if (index == gap && block_index+1 < block_count) {
                block++;
                index = 0;
                gap = block->cur;
                ++block_index;
              }
              return current;
            }
    };

    PoolBlock* blocks;
    short count = 1, capacity = 1;

    // unlike RawPool, not likely to see discarded space here
    T* _top() {
        if (!blocks[count-1].available()) {
            PoolBlock* old_blocks = blocks;
            capacity <<= 1;
            blocks = new PoolBlock[capacity];
            for (int i = 0; i < count; i++) blocks[i] = old_blocks[i];
            blocks[count++] = PoolBlock(DEFAULT_POOL_CAPACITY);
            delete[] old_blocks;
        }

        PoolBlock& top = blocks[count-1];
        return top.buf + top.cur++;
    }

    public:
        PoolBlock operator[](short i) {
            return blocks[i];
        }

        short block_count() const noexcept { return count; }

        Pool(): blocks(new PoolBlock[1]) { blocks[0] = PoolBlock(DEFAULT_POOL_CAPACITY); }

        ~Pool() {
            for (int i = 0; i < count; i++) delete[] blocks[i].buf;
            delete[] blocks;
        }

        template <typename... args> T* emplace(args&&... params) {
            return new (_top()) T(std::forward<args>(params)...);
        }

        T* insert(const T& item) {
            return new (_top()) T(item);
        }
};