#ifndef SEARCH_ALGORITHMS_H
#define SEARCH_ALGORITHMS_H

#include "Fault.h"
#include "Location.h"
#include <iostream>
#include <string>

// ============================================================
// MANUAL SEARCH ALGORITHMS FOR GRIDGUARD
// Linear Search: O(n) - works on unsorted data
// Binary Search: O(log n) - requires SORTED data
// ============================================================

class SearchAlgorithms {
public:
    // ============================================================
    // LINEAR SEARCH on Fault array
    // Scans every element from left to right until target found.
    // Time Complexity: O(n) - examines up to n elements
    // Space Complexity: O(1) - no extra memory needed
    // Works on UNSORTED data.
    // ============================================================
    static int linearSearchFaultById(const Fault arr[], int n, int targetFaultId) {
        for (int i = 0; i < n; ++i) {
            if (arr[i].faultId == targetFaultId) {
                return i; // Return index of found element
            }
        }
        return -1; // Not found
    }

    static int linearSearchFaultByLineId(const Fault arr[], int n, int targetLineId) {
        for (int i = 0; i < n; ++i) {
            if (arr[i].powerLineId == targetLineId) {
                return i;
            }
        }
        return -1;
    }

    // Linear search on Location array by ID
    static int linearSearchLocationById(const Location arr[], int n, int targetId) {
        for (int i = 0; i < n; ++i) {
            if (arr[i].id == targetId) {
                return i;
            }
        }
        return -1;
    }

    // Linear search on Location array by name keyword
    static int linearSearchLocationByName(const Location arr[], int n, const std::string& keyword) {
        for (int i = 0; i < n; ++i) {
            // Case-insensitive substring match
            std::string locName = arr[i].name;
            std::string kw = keyword;
            // Simple lowercase conversion
            for (char& c : locName) if (c >= 'A' && c <= 'Z') c += 32;
            for (char& c : kw)     if (c >= 'A' && c <= 'Z') c += 32;
            if (locName.find(kw) != std::string::npos) {
                return i;
            }
        }
        return -1;
    }

    // ============================================================
    // BINARY SEARCH on Fault array sorted by faultId (ascending)
    // IMPORTANT: Array MUST be sorted by faultId before calling!
    // Divides search space in half each iteration.
    // Time Complexity: O(log n) - much faster than linear for large n
    // Space Complexity: O(1)
    // ============================================================
    static int binarySearchFaultById(const Fault arr[], int n, int targetFaultId) {
        int low = 0;
        int high = n - 1;

        while (low <= high) {
            int mid = low + (high - low) / 2; // Avoid integer overflow vs (low+high)/2

            if (arr[mid].faultId == targetFaultId) {
                return mid; // Found at index mid
            } else if (arr[mid].faultId < targetFaultId) {
                low = mid + 1; // Target is in RIGHT half
            } else {
                high = mid - 1; // Target is in LEFT half
            }
        }
        return -1; // Not found
    }

    // Binary search by priority score (array must be sorted by priorityScore ascending)
    // Returns first index where priorityScore >= targetScore
    static int binarySearchFaultByScore(const Fault arr[], int n, double targetScore) {
        int low = 0;
        int high = n - 1;
        int result = -1;

        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (arr[mid].priorityScore >= targetScore) {
                result = mid;
                high = mid - 1; // Search left half for earlier occurrence
            } else {
                low = mid + 1; // Search right half
            }
        }
        return result;
    }

    // ============================================================
    // DEMONSTRATION: Display search comparison
    // Shows linear vs binary search performance
    // ============================================================
    static void demonstrateSearchComparison(const Fault arr[], int n, int targetFaultId) {
        std::cout << "\n--- Search Algorithm Comparison ---\n";
        std::cout << "Target Fault ID: " << targetFaultId << "\n";
        std::cout << "Array size: " << n << " fault records\n\n";

        // Linear Search
        int linearSteps = 0;
        int linearResult = -1;
        for (int i = 0; i < n; ++i) {
            linearSteps++;
            if (arr[i].faultId == targetFaultId) {
                linearResult = i;
                break;
            }
        }
        std::cout << "LINEAR SEARCH: Examined " << linearSteps << " element(s). ";
        if (linearResult != -1) std::cout << "Found at index " << linearResult << ".\n";
        else std::cout << "Not found.\n";

        // Binary Search (requires sorted array by faultId)
        // Create sorted copy
        Fault* sorted = new Fault[n];
        for (int i = 0; i < n; ++i) sorted[i] = arr[i];
        // Simple insertion sort by faultId for demo
        for (int i = 1; i < n; ++i) {
            Fault key = sorted[i];
            int j = i - 1;
            while (j >= 0 && sorted[j].faultId > key.faultId) {
                sorted[j + 1] = sorted[j];
                j--;
            }
            sorted[j + 1] = key;
        }

        int binarySteps = 0;
        int low = 0, high = n - 1, binaryResult = -1;
        while (low <= high) {
            binarySteps++;
            int mid = low + (high - low) / 2;
            if (sorted[mid].faultId == targetFaultId) { binaryResult = mid; break; }
            else if (sorted[mid].faultId < targetFaultId) low = mid + 1;
            else high = mid - 1;
        }
        std::cout << "BINARY SEARCH: Examined " << binarySteps << " element(s). ";
        if (binaryResult != -1) std::cout << "Found at index " << binaryResult << " (sorted).\n";
        else std::cout << "Not found.\n";
        std::cout << "Efficiency gain: Binary search used "
                  << linearSteps - binarySteps << " fewer comparisons.\n";

        delete[] sorted;
    }
};

#endif // SEARCH_ALGORITHMS_H
