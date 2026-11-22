#ifndef STACK_H
#define STACK_H

#include <stdexcept>
#include <iostream>
#include <string>

// Enum for types of operations that can be undone
enum UndoActionType {
    ACTION_LINE_DISABLED,
    ACTION_LINE_RESTORED,
    ACTION_FAULT_STATUS_CHANGED
};

struct UndoAction {
    UndoActionType type;
    int targetId;            // e.g., powerLineId or faultId
    int secondaryId;         // e.g., previous status or auxiliary info
    std::string description;

    UndoAction()
        : type(ACTION_LINE_DISABLED), targetId(-1), secondaryId(0), description("") {}

    UndoAction(UndoActionType t, int target, int sec, const std::string& desc)
        : type(t), targetId(target), secondaryId(sec), description(desc) {}

    void display() const {
        std::cout << "Action: " << description << " [Target ID: " << targetId << "]";
    }
};

// Manual LIFO Stack implementation using a Singly Linked List
template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val, Node* nxt = nullptr) : data(val), next(nxt) {}
    };

    Node* topNode;
    int count;

public:
    Stack() : topNode(nullptr), count(0) {}

    ~Stack() {
        clear();
    }

    Stack(const Stack& other) : topNode(nullptr), count(0) {
        // Need to copy maintaining LIFO order
        if (other.topNode == nullptr) return;
        
        // Helper array or temporary stack to reverse
        Node* curr = other.topNode;
        Stack<T> temp;
        while (curr != nullptr) {
            temp.push(curr->data);
            curr = curr->next;
        }
        while (!temp.isEmpty()) {
            push(temp.pop());
        }
    }

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            clear();
            if (other.topNode != nullptr) {
                Stack<T> temp;
                Node* curr = other.topNode;
                while (curr != nullptr) {
                    temp.push(curr->data);
                    curr = curr->next;
                }
                while (!temp.isEmpty()) {
                    push(temp.pop());
                }
            }
        }
        return *this;
    }

    void push(const T& value) {
        topNode = new Node(value, topNode);
        count++;
    }

    T pop() {
        if (isEmpty()) {
            throw std::runtime_error("Cannot pop from an empty Stack");
        }
        Node* temp = topNode;
        T val = temp->data;
        topNode = topNode->next;
        delete temp;
        count--;
        return val;
    }

    const T& peek() const {
        if (isEmpty()) {
            throw std::runtime_error("Cannot peek into an empty Stack");
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    int size() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }

    void displayHistory() const {
        if (isEmpty()) {
            std::cout << "   [Undo Stack is Empty - No reversible operations recorded]\n";
            return;
        }
        std::cout << "   Current Undo Stack (Top -> Bottom, " << count << " entries):\n";
        Node* curr = topNode;
        int level = 1;
        while (curr != nullptr) {
            std::cout << "   " << level++ << ". ";
            curr->data.display();
            std::cout << "\n";
            curr = curr->next;
        }
    }
};

#endif // STACK_H
