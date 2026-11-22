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

    // Helper to get criterion label
    static std::string getCriterionName(SortCriterion criterion);
};

#endif // SORTING_H
