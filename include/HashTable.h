#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>

// Manual Hash Table implementation using Separate Chaining (Linked Lists)
template <typename K, typename V>
class HashTable {
private:
    struct HashNode {
        K key;
        V value;
        HashNode* next;

        HashNode(const K& k, const V& v, HashNode* nxt = nullptr)
            : key(k), value(v), next(nxt) {}
    };

    HashNode** buckets;
    int capacity;
    int totalElements;
    int collisionCount;

    // Hash function for integer keys
    int hashFunction(int key) const {
        int hash = key % capacity;
        return hash < 0 ? hash + capacity : hash;
    }

    // Hash function for string keys (djb2 hash algorithm)
    int hashFunction(const std::string& key) const {
        unsigned long hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        return static_cast<int>(hash % capacity);
    }

public:
    explicit HashTable(int cap = 17)
        : capacity(cap > 0 ? cap : 17), totalElements(0), collisionCount(0) {
        buckets = new HashNode*[capacity];
        for (int i = 0; i < capacity; ++i) {
            buckets[i] = nullptr;
        }
    }

    ~HashTable() {
        clear();
        delete[] buckets;
    }

    HashTable(const HashTable& other)
        : capacity(other.capacity), totalElements(0), collisionCount(0) {
        buckets = new HashNode*[capacity];
        for (int i = 0; i < capacity; ++i) {
            buckets[i] = nullptr;
        }
        for (int i = 0; i < other.capacity; ++i) {
            HashNode* curr = other.buckets[i];
            while (curr != nullptr) {
                insert(curr->key, curr->value);
                curr = curr->next;
            }
        }
    }

    HashTable& operator=(const HashTable& other) {
        if (this != &other) {
            clear();
            delete[] buckets;

            capacity = other.capacity;
            totalElements = 0;
            collisionCount = 0;
            buckets = new HashNode*[capacity];
            for (int i = 0; i < capacity; ++i) {
                buckets[i] = nullptr;
            }
            for (int i = 0; i < other.capacity; ++i) {
                HashNode* curr = other.buckets[i];
                while (curr != nullptr) {
                    insert(curr->key, curr->value);
                    curr = curr->next;
                }
            }
        }
        return *this;
    }

    // Insert or update key-value pair. Separate chaining handles collisions.
    bool insert(const K& key, const V& value) {
        int index = hashFunction(key);
        HashNode* curr = buckets[index];

        // Check if key already exists, update value
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->value = value;
                return false; // Updated existing key
            }
            curr = curr->next;
        }

        // Detect collision: if bucket is not empty, this is a collision
        if (buckets[index] != nullptr) {
            collisionCount++;
        }

        // Insert at beginning of linked list at this bucket
        HashNode* newNode = new HashNode(key, value, buckets[index]);
        buckets[index] = newNode;
        totalElements++;
        return true; // Newly inserted
    }

    // Search by key, copies value to outValue if found
    bool search(const K& key, V& outValue) const {
        int index = hashFunction(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                outValue = curr->value;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    // Search by key and return pointer to value for in-place modifications
    V* searchPtr(const K& key) {
        int index = hashFunction(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    const V* searchPtr(const K& key) const {
        int index = hashFunction(key);
        HashNode* curr = buckets[index];
        while (curr != nullptr) {
            if (curr->key == key) {
                return &(curr->value);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    bool update(const K& key, const V& newValue) {
        V* ptr = searchPtr(key);
        if (ptr != nullptr) {
            *ptr = newValue;
            return true;
        }
        return false;
    }

    bool remove(const K& key) {
        int index = hashFunction(key);
        HashNode* curr = buckets[index];
        HashNode* prev = nullptr;

        while (curr != nullptr) {
            if (curr->key == key) {
                if (prev == nullptr) {
                    buckets[index] = curr->next;
                } else {
                    prev->next = curr->next;
                }
                delete curr;
                totalElements--;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    bool contains(const K& key) const {
        V dummy;
        return search(key, dummy);
    }

    int size() const {
        return totalElements;
    }

    int getCapacity() const {
        return capacity;
    }

    int getCollisionCount() const {
        return collisionCount;
    }

    void clear() {
        for (int i = 0; i < capacity; ++i) {
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                HashNode* temp = curr;
                curr = curr->next;
                delete temp;
            }
            buckets[i] = nullptr;
        }
        totalElements = 0;
        collisionCount = 0;
    }

    // Diagnostics to display collision distribution across buckets
    void displayBucketDiagnostics() const {
        std::cout << "HashTable Diagnostics (Capacity: " << capacity
                  << ", Elements: " << totalElements
                  << ", Collisions: " << collisionCount << "):\n";
        for (int i = 0; i < capacity; ++i) {
            int chainLen = 0;
            HashNode* curr = buckets[i];
            while (curr != nullptr) {
                chainLen++;
                curr = curr->next;
            }
            if (chainLen > 0) {
                std::cout << "   Bucket [" << std::setw(2) << i << "]: "
                          << chainLen << " item(s)";
                if (chainLen > 1) {
                    std::cout << " (COLLISION CHAIN DETECTED)";
                }
                std::cout << "\n";
            }
        }
    }
};

#endif // HASH_TABLE_H
