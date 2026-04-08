#include <cstring>
#include <string>

class TextView {
    const char* _data;
    size_t _len;

    public:
        size_t size() const { return _len; }
        const char* data() const { return _data; }
        TextView(): _data(nullptr), _len(0) {}
        TextView(const char* ref, size_t from, size_t until): _data(&ref[from]), _len(until-from) {};
        TextView(const char* a, const char* b): _data(a), _len(b-a) {}

        bool operator==(const TextView& other) const {
            return _len == other._len && std::memcmp(_data, other._data, _len) == 0;
        }

        bool operator==(const std::string& other) const {
            return _len == other.size() && std::memcmp(_data, other.data(), _len) == 0;
        }
};