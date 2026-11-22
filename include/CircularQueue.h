#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdexcept>
#include <iostream>

// ============================================================
// MANUAL CIRCULAR QUEUE — Array-Based Implementation
// Also known as Ring Buffer or Circular Buffer
//
// A circular queue reuses empty slots at the front by wrapping
// the rear pointer around to the beginning using modulo arithmetic.
//
// Key Variables:
//   front  - index of the first (oldest) element
//   rear   - index where the NEXT element will be inserted
//   size_  - current number of elements
//   cap    - maximum capacity (fixed at construction)
//   data[] - raw array storing queue elements
//
// Circular Wrap: rear = (rear + 1) % cap
//                front = (front + 1) % cap
//
// Time Complexity:
//   enqueue: O(1) - just increment rear and store
//   dequeue: O(1) - just increment front and return
//   peek:    O(1) - read data[front]
// ============================================================
template <typename T>
class CircularQueue {
private:
    T* data;      // Dynamically allocated array for queue storage
    int front_;   // Index of the front element (oldest)
    int rear_;    // Index where next element will be placed
    int size_;    // Current number of elements in the queue
    int cap;      // Maximum capacity of this queue

public:
    // Constructor: allocate array of given capacity
    explicit CircularQueue(int capacity = 64)
        : front_(0), rear_(0), size_(0), cap(capacity > 0 ? capacity : 64) {
        data = new T[cap];
    }

    // Destructor: release allocated array memory
    ~CircularQueue() {
        delete[] data;
    }

    // Copy constructor: deep copy of circular queue
    CircularQueue(const CircularQueue& other)
        : front_(other.front_), rear_(other.rear_),
          size_(other.size_), cap(other.cap) {
        data = new T[cap];
        // Copy all elements from other's array
        for (int i = 0; i < cap; ++i) {
            data[i] = other.data[i];
        }
    }

    // Assignment operator
    CircularQueue& operator=(const CircularQueue& other) {
        if (this != &other) {
            delete[] data;
            front_ = other.front_;
            rear_  = other.rear_;
            size_  = other.size_;
            cap    = other.cap;
            data   = new T[cap];
            for (int i = 0; i < cap; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // ============================================================
    // ENQUEUE: Add element at the REAR
    // rear = (rear + 1) % cap  <-- this is the circular wrap!
    // ============================================================
    void enqueue(const T& value) {
        if (isFull()) {
            throw std::runtime_error("CircularQueue is FULL - cannot enqueue");
        }
        data[rear_] = value;                // Store element at rear position
        rear_ = (rear_ + 1) % cap;         // Advance rear with circular wrap
        size_++;
    }

    // ============================================================
    // DEQUEUE: Remove and return element from FRONT
    // front = (front + 1) % cap  <-- circular wrap at front
    // ============================================================
    T dequeue() {
        if (isEmpty()) {
            throw std::runtime_error("CircularQueue is EMPTY - cannot dequeue");
        }
        T val = data[front_];              // Save front element
        front_ = (front_ + 1) % cap;      // Advance front with circular wrap
        size_--;
        return val;
    }

    // PEEK: Return front element without removing it
    const T& peek() const {
        if (isEmpty()) {
            throw std::runtime_error("CircularQueue is EMPTY - cannot peek");
        }
        return data[front_];
    }

    bool isEmpty() const { return size_ == 0; }
    bool isFull()  const { return size_ == cap; }
    int  size()    const { return size_; }
    int  capacity() const { return cap; }

    // Clear all elements by resetting indices
    void clear() {
        front_ = 0;
        rear_  = 0;
        size_  = 0;
    }

    // Display queue contents from front to rear
    void display() const {
        if (isEmpty()) {
            std::cout << "[CircularQueue is Empty]\n";
            return;
        }
        std::cout << "CircularQueue (Front -> Rear, " << size_ << "/" << cap << " slots): ";
        for (int i = 0; i < size_; ++i) {
            // Calculate actual array index with circular offset
            int idx = (front_ + i) % cap;
            std::cout << data[idx];
            if (i + 1 < size_) std::cout << " -> ";
        }
        std::cout << "\n";
        std::cout << "  [front_=" << front_ << ", rear_=" << rear_
                  << ", size=" << size_ << ", cap=" << cap << "]\n";
    }
};

#endif // CIRCULAR_QUEUE_H
