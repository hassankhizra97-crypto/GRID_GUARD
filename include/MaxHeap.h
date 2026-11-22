#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "Fault.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// Custom Max-Heap implementation for Fault Priority Scheduling
class MaxHeap {
private:
    Fault* heap;
    int capacity;
    int heapSize;

    void resize(int newCapacity) {
        Fault* newHeap = new Fault[newCapacity];
        for (int i = 0; i < heapSize; ++i) {
            newHeap[i] = heap[i];
        }
        delete[] heap;
        heap = newHeap;
        capacity = newCapacity;
    }

    int parent(int i) const { return (i - 1) / 2; }
    int leftChild(int i) const { return (2 * i) + 1; }
    int rightChild(int i) const { return (2 * i) + 2; }

    void swap(int i, int j) {
        Fault temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }

public:
    explicit MaxHeap(int initialCapacity = 20)
        : capacity(initialCapacity > 0 ? initialCapacity : 20), heapSize(0) {
        heap = new Fault[capacity];
    }

    ~MaxHeap() {
        delete[] heap;
    }

    MaxHeap(const MaxHeap& other)
        : capacity(other.capacity), heapSize(other.heapSize) {
        heap = new Fault[capacity];
        for (int i = 0; i < heapSize; ++i) {
            heap[i] = other.heap[i];
        }
    }

    MaxHeap& operator=(const MaxHeap& other) {
        if (this != &other) {
            delete[] heap;
            capacity = other.capacity;
            heapSize = other.heapSize;
            heap = new Fault[capacity];
            for (int i = 0; i < heapSize; ++i) {
                heap[i] = other.heap[i];
            }
        }
        return *this;
    }

    // Percolate up to restore max-heap property
    void heapifyUp(int i) {
        while (i > 0 && heap[i].priorityScore > heap[parent(i)].priorityScore) {
            swap(i, parent(i));
            i = parent(i);
        }
    }

    // Percolate down to restore max-heap property
    void heapifyDown(int i) {
        int largest = i;
        int left = leftChild(i);
        int right = rightChild(i);

        if (left < heapSize && heap[left].priorityScore > heap[largest].priorityScore) {
            largest = left;
        }
        if (right < heapSize && heap[right].priorityScore > heap[largest].priorityScore) {
            largest = right;
        }

        if (largest != i) {
            swap(i, largest);
            heapifyDown(largest);
        }
    }

    void insert(const Fault& fault) {
        if (heapSize >= capacity) {
            resize(capacity * 2);
        }
        heap[heapSize] = fault;
        heapifyUp(heapSize);
        heapSize++;
    }

    Fault extractMax() {
        if (isEmpty()) {
            throw std::runtime_error("Cannot extractMax from an empty Max-Heap");
        }
        Fault maxItem = heap[0];
        heap[0] = heap[heapSize - 1];
        heapSize--;
        if (heapSize > 0) {
            heapifyDown(0);
        }
        return maxItem;
    }

    const Fault& peekMax() const {
        if (isEmpty()) {
            throw std::runtime_error("Cannot peekMax from an empty Max-Heap");
        }
        return heap[0];
    }

    bool isEmpty() const {
        return heapSize == 0;
    }

    int size() const {
        return heapSize;
    }

    void clear() {
        heapSize = 0;
    }

    // Update fault status in-place if matched
    bool updateFaultStatus(int faultId, FaultStatus newStatus) {
        for (int i = 0; i < heapSize; ++i) {
            if (heap[i].faultId == faultId) {
                heap[i].status = newStatus;
                return true;
            }
        }
        return false;
    }

    Fault* findFault(int faultId) {
        for (int i = 0; i < heapSize; ++i) {
            if (heap[i].faultId == faultId) {
                return &heap[i];
            }
        }
        return nullptr;
    }

    // Display the entire repair queue in priority order without mutating original heap
    void displayQueue() const {
        if (isEmpty()) {
            std::cout << "   [Emergency Repair Queue is Empty - All lines operational]\n";
            return;
        }

        std::cout << "========================================================================================\n";
        std::cout << "                       EMERGENCY REPAIR PRIORITY QUEUE (MAX-HEAP)                       \n";
        std::cout << "========================================================================================\n";
        std::cout << std::left
                  << std::setw(6)  << "Rank"
                  << std::setw(10) << "Fault ID"
                  << std::setw(10) << "Line ID"
                  << std::setw(10) << "Severity"
                  << std::setw(12) << "Crit Fac"
                  << std::setw(14) << "Pop Affected"
                  << std::setw(12) << "Duration"
                  << std::setw(16) << "Priority Score"
                  << std::setw(12) << "Status"
                  << "\n";
        std::cout << "----------------------------------------------------------------------------------------\n";

        // Create temporary copy of heap to extract and display in strict descending order
        MaxHeap tempHeap(*this);
        int rank = 1;
        while (!tempHeap.isEmpty()) {
            Fault f = tempHeap.extractMax();
            std::cout << std::left
                      << std::setw(6)  << rank++
                      << std::setw(10) << f.faultId
                      << std::setw(10) << f.powerLineId
                      << std::setw(10) << f.severity
                      << std::setw(12) << f.criticalFacilitiesAffected
                      << std::setw(14) << f.populationAffected
                      << (std::to_string(f.outageDuration) + "m     ").substr(0, 12)
                      << std::setw(16) << std::fixed << std::setprecision(1) << f.priorityScore
                      << std::setw(12) << f.getStatusString()
                      << "\n";
        }
        std::cout << "========================================================================================\n";
    }
};

#endif // MAX_HEAP_H
