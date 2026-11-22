#ifndef SORTING_H
#define SORTING_H

#include "Fault.h"
#include <string>

enum SortCriterion {
    SORT_BY_PRIORITY_SCORE,
    SORT_BY_SEVERITY,
    SORT_BY_POPULATION,
    SORT_BY_OUTAGE_DURATION
};

enum SortAlgorithm {
    ALGO_BUBBLE_SORT = 1,
    ALGO_SELECTION_SORT = 2,
    ALGO_INSERTION_SORT = 3,
    ALGO_QUICK_SORT = 4,
    ALGO_MERGE_SORT = 5
};

class Sorting {
private:
    // Partition function for QuickSort
    static int partition(Fault arr[], int low, int high, SortCriterion criterion, bool ascending);

    // Merge helper for MergeSort
    static void merge(Fault arr[], int left, int mid, int right, SortCriterion criterion, bool ascending);

    // Comparator function
    static bool compare(const Fault& a, const Fault& b, SortCriterion criterion, bool ascending);

public:
    // Manual QuickSort implementation
    static void quickSort(Fault arr[], int low, int high, SortCriterion criterion, bool ascending = false);

    // Manual MergeSort implementation
    static void mergeSort(Fault arr[], int left, int right, SortCriterion criterion, bool ascending = false);

    // Manual Bubble Sort - O(n^2) - good for small arrays, easy to understand
    static void bubbleSort(Fault arr[], int n, SortCriterion criterion, bool ascending = false);

    // Manual Selection Sort - O(n^2) - finds minimum each pass
    static void selectionSort(Fault arr[], int n, SortCriterion criterion, bool ascending = false);

    // Manual Insertion Sort - O(n^2) avg, O(n) best - efficient for nearly-sorted data
    static void insertionSort(Fault arr[], int n, SortCriterion criterion, bool ascending = false);

    // Sort algorithm selector by name
    static std::string getSortAlgorithmName(int algo); // 1=Bubble 2=Selection 3=Insertion 4=Quick 5=Merge

    // Helper to get criterion label
    static std::string getCriterionName(SortCriterion criterion);
};

#endif // SORTING_H
