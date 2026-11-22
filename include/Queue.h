#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>
#include <iostream>

// Manual FIFO Queue implementation using a Singly Linked List
template <typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* frontNode;
    Node* rearNode;
    int count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~Queue() {
        clear();
    }

    Queue(const Queue& other) : frontNode(nullptr), rearNode(nullptr), count(0) {
        Node* curr = other.frontNode;
        while (curr != nullptr) {
            enqueue(curr->data);
            curr = curr->next;
        }
    }

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            clear();
            Node* curr = other.frontNode;
            while (curr != nullptr) {
                enqueue(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    void enqueue(const T& value) {
        Node* newNode = new Node(value);
        if (rearNode == nullptr) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::runtime_error("Cannot dequeue from an empty Queue");
        }
        Node* temp = frontNode;
        T val = temp->data;
        frontNode = frontNode->next;
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        count--;
        return val;
    }

    const T& peek() const {
        if (isEmpty()) {
            throw std::runtime_error("Cannot peek into an empty Queue");
        }
        return frontNode->data;
    }

    bool isEmpty() const {
        return frontNode == nullptr;
    }

    int size() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    // Traverse and display the elements in the queue
    void display() const {
        if (isEmpty()) {
            std::cout << "[Queue is Empty]\n";
            return;
        }
        Node* curr = frontNode;
        std::cout << "Queue (Front -> Rear): ";
        while (curr != nullptr) {
            std::cout << curr->data;
            if (curr->next != nullptr) std::cout << " -> ";
            curr = curr->next;
        }
        std::cout << "\n";
    }
};

#endif // QUEUE_H
