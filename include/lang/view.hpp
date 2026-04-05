#include <cstring>
#include <string>

class TextPreview {
    const char* data;
    size_t length;

    public:
        size_t size() const { return length; }
        TextPreview(const char* ref, size_t from, size_t until): data(&ref[from]), length(until-from) {};

        bool operator==(const TextPreview& other) const {
            return length == other.length && std::memcmp(data, other.data, length) == 0;
        }

        bool operator==(const std::string& other) const {
            return length == other.length() && std::memcmp(data, other.data(), length) == 0;
        }
};