#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <algorithm> 
#include <stdexcept>

namespace util {

template <class T, size_t N>
struct dynamic_array {
private:
    std::array<T, N> arr_{};
    size_t size_{};

public:
    static constexpr size_t capacity() { return N; }
    bool full()  const { return size_ == capacity(); }
    bool empty() const { return size_ == 0; }
    size_t size() const { return size_; }

    dynamic_array() = default;

    explicit dynamic_array(std::initializer_list<T> il) : size_(il.size()) {
        assert(il.size() <= N);
        std::ranges::copy(il, arr_.begin());
    }

    T& at(const size_t i) {
        if (i >= size_) { throw std::out_of_range("Error, .at() access out of bounds."); }
        return arr_[i];
    }

    const T& at(const size_t i) const {
        if (i >= size_) { throw std::out_of_range("Error, .at() access out of bounds."); }
        return arr_[i];
    }

    bool contains(const T& m) const {
        for (size_t i{}; i < size_; i++) {
            if (m == arr_[i]) { return true; }
        }
        return false;
    }

    size_t find (const T& m) const {
        for (size_t i{}; i < size_; i++) {
            if (m == arr_[i]) { return i; }
        }
        return size_;
    }

    void push_back(const T& m) {
        assert(size_ < capacity());
        arr_[size_++] = m;
    }
};

} // namespace util