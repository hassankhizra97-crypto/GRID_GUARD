#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include "IncidentEvent.h"
#include <iostream>
#include <iomanip>
#include <string>

// Custom Doubly Linked List for Incident History tracking
template <typename T>
class DoublyLinkedList {
private:
    struct DNode {
        T data;
        DNode* prev;
        DNode* next;

        DNode(const T& val, DNode* p = nullptr, DNode* n = nullptr)
            : data(val), prev(p), next(n) {}
    };

    DNode* head;
    DNode* tail;
    int count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~DoublyLinkedList() {
        clear();
    }

    DoublyLinkedList(const DoublyLinkedList& other)
        : head(nullptr), tail(nullptr), count(0) {
        DNode* curr = other.head;
        while (curr != nullptr) {
            append(curr->data);
            curr = curr->next;
        }
    }

    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        if (this != &other) {
            clear();
            DNode* curr = other.head;
            while (curr != nullptr) {
                append(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    void append(const T& value) {
        DNode* newNode = new DNode(value, tail, nullptr);
        if (tail != nullptr) {
            tail->next = newNode;
        } else {
            head = newNode;
        }
        tail = newNode;
        count++;
    }

    void prepend(const T& value) {
        DNode* newNode = new DNode(value, nullptr, head);
        if (head != nullptr) {
            head->prev = newNode;
        } else {
            tail = newNode;
        }
        head = newNode;
        count++;
    }

    // Remove the oldest record.  Both neighbour links are repaired before the
    // old node is released, so the list stays valid in both directions.
    bool removeFirst() {
        if (head == nullptr) return false;
        DNode* oldHead = head;
        head = head->next;
        if (head != nullptr) head->prev = nullptr;
        else tail = nullptr;
        delete oldHead;
        --count;
        return true;
    }

    // Remove the newest record in O(1), using the backward pointer.
    bool removeLast() {
        if (tail == nullptr) return false;
        DNode* oldTail = tail;
        tail = tail->prev;
        if (tail != nullptr) tail->next = nullptr;
        else head = nullptr;
        delete oldTail;
        --count;
        return true;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    int size() const {
        return count;
    }

    const T* getLatest() const {
        if (tail == nullptr) return nullptr;
        return &(tail->data);
    }

    void clear() {
        DNode* curr = head;
        while (curr != nullptr) {
            DNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
        head = tail = nullptr;
        count = 0;
    }

    // Traverse forward from oldest to newest event
    void displayForward() const {
        if (isEmpty()) {
            std::cout << "   [Incident History is Empty - No events recorded]\n";
            return;
        }
        std::cout << "========================================================================================\n";
        std::cout << "                  INCIDENT HISTORY LOG (CHRONOLOGICAL: OLDEST -> NEWEST)                 \n";
        std::cout << "========================================================================================\n";
        DNode* curr = head;
        while (curr != nullptr) {
            curr->data.display();
            curr = curr->next;
        }
        std::cout << "========================================================================================\n";
    }

    // Traverse backward from newest to oldest event
    void displayBackward() const {
        if (isEmpty()) {
            std::cout << "   [Incident History is Empty - No events recorded]\n";
            return;
        }
        std::cout << "========================================================================================\n";
        std::cout << "              INCIDENT HISTORY LOG (REVERSE CHRONOLOGICAL: NEWEST -> OLDEST)            \n";
        std::cout << "========================================================================================\n";
        DNode* curr = tail;
        while (curr != nullptr) {
            curr->data.display();
            curr = curr->prev;
        }
        std::cout << "========================================================================================\n";
    }

    // Iterate over all entries from head to tail (for JSON export / server integration)
    template <typename Visitor>
    void forEach(Visitor visit) const {
        DNode* curr = head;
        while (curr != nullptr) {
            visit(curr->data);
            curr = curr->next;
        }
    }

    // Search events containing a keyword in their description or type
    void searchEvents(const std::string& keyword) const {
        if (isEmpty()) {
            std::cout << "   [Incident History is Empty]\n";
            return;
        }
        std::cout << "--- Search Results for Keyword: \"" << keyword << "\" ---\n";
        DNode* curr = head;
        int found = 0;
        while (curr != nullptr) {
            if (curr->data.description.find(keyword) != std::string::npos ||
                curr->data.eventType.find(keyword) != std::string::npos) {
                curr->data.display();
                found++;
            }
            curr = curr->next;
        }
        if (found == 0) {
            std::cout << "   No events matched \"" << keyword << "\".\n";
        } else {
            std::cout << "   Total matched events: " << found << "\n";
        }
    }
};

#endif // DOUBLY_LINKED_LIST_H
