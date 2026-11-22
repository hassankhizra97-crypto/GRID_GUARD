#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include <stdexcept>
#include <iostream>
#include <string>

// ============================================================
// MANUAL ARRAY-BASED STACK — LIFO (Last In First Out)
//
// Uses a dynamically allocated array with a 'top' index.
// Push increments top; Pop decrements top.
// When full, doubles capacity (dynamic resizing).
//
// Key Variables:
//   data[]   - array holding stack elements
//   topIdx   - index of the current top element (-1 if empty)
//   cap      - current maximum capacity
//
// Time Complexity:
//   push: O(1) amortized (O(n) when resizing, rarely)
//   pop:  O(1)
//   peek: O(1)
//
// Space Complexity: O(n)
// ============================================================
template <typename T>
class ArrayStack {
private:
    T*  data;    // Dynamically allocated array for stack storage
    int topIdx;  // Index of top element; -1 means stack is empty
    int cap;     // Current maximum capacity

    // Resize the internal array when capacity is exceeded
    void resize(int newCap) {
        T* newData = new T[newCap];
        // Copy existing elements to new array
        for (int i = 0; i <= topIdx; ++i) {
            newData[i] = data[i];
        }
        delete[] data;    // Release old memory
        data = newData;   // Point to new array
        cap  = newCap;
    }

public:
    // Constructor
    explicit ArrayStack(int initialCap = 32)
        : topIdx(-1), cap(initialCap > 0 ? initialCap : 32) {
        data = new T[cap];
    }

    // Destructor
    ~ArrayStack() {
        delete[] data;
    }

    // Copy constructor
    ArrayStack(const ArrayStack& other)
        : topIdx(other.topIdx), cap(other.cap) {
        data = new T[cap];
        for (int i = 0; i <= topIdx; ++i) {
            data[i] = other.data[i];
        }
    }

    // Assignment operator
    ArrayStack& operator=(const ArrayStack& other) {
        if (this != &other) {
            delete[] data;
            topIdx = other.topIdx;
            cap    = other.cap;
            data   = new T[cap];
            for (int i = 0; i <= topIdx; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // ============================================================
    // PUSH: Place element on top of stack
    // Increment top index, then store element.
    // If array is full, resize to double capacity first.
    // ============================================================
    void push(const T& value) {
        if (topIdx + 1 >= cap) {
            resize(cap * 2); // Double capacity when full
        }
        data[++topIdx] = value; // Increment top, then store
    }

    // ============================================================
    // POP: Remove and return element from top of stack
    // Save top element, then decrement top index.
    // ============================================================
    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("ArrayStack is EMPTY - cannot pop");
        }
        return data[topIdx--]; // Return top element, then decrement
    }

    // PEEK: Return top element without removing
    const T& peek() const {
        if (isEmpty()) {
            throw std::runtime_error("ArrayStack is EMPTY - cannot peek");
        }
        return data[topIdx];
    }

    bool isEmpty() const { return topIdx == -1; }
    bool isFull()  const { return topIdx + 1 >= cap; }
    int  size()    const { return topIdx + 1; }
    int  capacity() const { return cap; }

    // Clear by resetting top index (data remains in array but is logically gone)
    void clear() {
        topIdx = -1;
    }

    // Visit elements from newest to oldest without changing the stack.
    // The callable receives a const reference to each stored value.
    template <typename Visitor>
    void forEachTopDown(Visitor visit) const {
        for (int i = topIdx; i >= 0; --i) {
            visit(data[i]);
        }
    }

    // Display stack from top to bottom
    void display() const {
        if (isEmpty()) {
            std::cout << "[ArrayStack is Empty]\n";
            return;
        }
        std::cout << "ArrayStack (Top -> Bottom, " << size() << " elements):\n";
        for (int i = topIdx; i >= 0; --i) {
            std::cout << "  [" << (topIdx - i + 1) << "] ";
            // Try to call display() if T has it, otherwise just print index
            std::cout << "\n";
        }
        std::cout << "  [top=" << topIdx << ", cap=" << cap << "]\n";
    }
};

#endif // ARRAY_STACK_H
