#ifndef CUSTOM_VECTOR_H
#define CUSTOM_VECTOR_H

#include <cstddef>
#include <stdexcept>
#include <utility>

// Custom dynamically-resizing array template to adhere to strict DSA project requirements (no STL containers)
template <typename T>
class CustomVector {
private:
    T* data;
    size_t capacity_;
    size_t size_;

    void resize(size_t newCapacity) {
        if (newCapacity < size_) {
            newCapacity = size_;
        }
        T* newData = new T[newCapacity];
        for (size_t i = 0; i < size_; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
        capacity_ = newCapacity;
    }

public:
    CustomVector() : data(nullptr), capacity_(0), size_(0) {
        resize(4);
    }

    explicit CustomVector(size_t initialCapacity) : data(nullptr), capacity_(0), size_(0) {
        resize(initialCapacity > 0 ? initialCapacity : 4);
    }

    CustomVector(const CustomVector& other) : data(nullptr), capacity_(other.capacity_), size_(other.size_) {
        data = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data[i] = other.data[i];
        }
    }

    CustomVector& operator=(const CustomVector& other) {
        if (this != &other) {
            delete[] data;
            capacity_ = other.capacity_;
            size_ = other.size_;
            data = new T[capacity_];
            for (size_t i = 0; i < size_; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    ~CustomVector() {
        delete[] data;
    }

    void push_back(const T& element) {
        if (size_ >= capacity_) {
            resize(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data[size_++] = element;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
        }
    }

    T& operator[](size_t index) {
        return data[index];
    }

    const T& operator[](size_t index) const {
        return data[index];
    }

    T& at(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("CustomVector index out of range");
        }
        return data[index];
    }

    const T& at(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("CustomVector index out of range");
        }
        return data[index];
    }

    size_t size() const {
        return size_;
    }

    size_t capacity() const {
        return capacity_;
    }

    bool empty() const {
        return size_ == 0;
    }

    void clear() {
        size_ = 0;
    }

    T* begin() { return data; }
    const T* begin() const { return data; }
    T* end() { return data + size_; }
    const T* end() const { return data + size_; }
};

#endif // CUSTOM_VECTOR_H
