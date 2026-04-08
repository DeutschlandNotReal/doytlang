#include <cstring>
#include <cstdio>
#include <iostream>
#include <chrono>

#include <lang/dlex.hpp>
#include <ostream>

template <int Q> bool match(const char* arg, const char (&to)[Q]) {
    int len = 0;
    while (*(arg++)) ++ len;
    if (len != Q - 1) return false;
    arg -= len + 1;
    return std::memcmp(arg,to, Q) == 0;
}

#define F_TOKPRINT 1
#define F_POOLPRINT 2
#define F_MEASURE 4

// flags: 
// [-tokprint]: prints out the tokens
// [-poolprint]: prints out pool status
// [-measure]: measures compilation times

int main(int count, const char** args) {
    int flags = 0;
    const char** srcs = new const char*[64];
    int char_count = 0;
    int src_index = 0;

    --count; ++args;
    while (count--) {
        const char* arg = *(args++);
        if (*arg == '-') {
            ++arg;
            if (match(arg, "tokprint")) { flags |= F_TOKPRINT; continue; }
            if (match(arg, "poolprint")) { flags |= F_POOLPRINT; continue; }
            if (match(arg, "measure")) { flags |= F_MEASURE; continue; }
            std::cout << "Unrecognized flag \"" << --arg << "\"\n";
        } else {
            FILE* f = fopen(arg, "r");
            if (!f) { std::cout << "File " << arg << " wasn't found\n"; continue; }
            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            char_count += size;
            rewind(f);
            char* src = new char[size+1];
            fread(src, 1, size, f);
            src[size] = '\0';
            fclose(f);
            srcs[src_index++] = src;
        }
    }

    if (!src_index) {
        std::cout << "No Source Files given.\n"; return 1;
    }
    std::cout << "Parsing " << src_index << " file(s) with " << char_count << " characters";

    auto parse_begin = std::chrono::steady_clock::now();
    LexOutput lexout = tokenize(srcs, src_index, flags);

    float ms_taken = 1000.f * std::chrono::duration<float>(std::chrono::steady_clock::now() - parse_begin).count();
    if (flags & F_MEASURE) {
        std::cout << 
        "Took " << ms_taken << " ms ("
        << (ms_taken / src_index) << " ms/file, "
        << (char_count / ms_taken) << " char/ms)\n";
    }

    if (flags & F_TOKPRINT) {
        std::cout << "Parsed " << lexout.count() << " tokens\n";
        auto tok_iter = lexout.token_pool.iterator();
        while (tok_iter.has_next()) {
            tok_iter.consume().print(std::cout) << ", ";
        }
    }
    
    return 0;

}