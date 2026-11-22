#include "../include/Sorting.h"

std::string Sorting::getCriterionName(SortCriterion criterion) {
    switch (criterion) {
        case SORT_BY_PRIORITY_SCORE:  return "Priority Score";
        case SORT_BY_SEVERITY:        return "Severity Level";
        case SORT_BY_POPULATION:      return "Population Affected";
        case SORT_BY_OUTAGE_DURATION: return "Outage Duration";
        default:                      return "Unknown";
    }
}

bool Sorting::compare(const Fault& a, const Fault& b, SortCriterion criterion, bool ascending) {
    double valA = 0.0;
    double valB = 0.0;

    switch (criterion) {
        case SORT_BY_PRIORITY_SCORE:
            valA = a.priorityScore;
            valB = b.priorityScore;
            break;
        case SORT_BY_SEVERITY:
            valA = static_cast<double>(a.severity);
            valB = static_cast<double>(b.severity);
            break;
        case SORT_BY_POPULATION:
            valA = static_cast<double>(a.populationAffected);
            valB = static_cast<double>(b.populationAffected);
            break;
        case SORT_BY_OUTAGE_DURATION:
            valA = static_cast<double>(a.outageDuration);
            valB = static_cast<double>(b.outageDuration);
            break;
    }

    if (ascending) {
        return valA < valB;
    } else {
        return valA > valB; // Descending default for emergency prioritization
    }
}

int Sorting::partition(Fault arr[], int low, int high, SortCriterion criterion, bool ascending) {
    Fault pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (compare(arr[j], pivot, criterion, ascending)) {
            i++;
            Fault temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Fault temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

// Manual QuickSort Implementation
void Sorting::quickSort(Fault arr[], int low, int high, SortCriterion criterion, bool ascending) {
    if (low < high) {
        int pi = partition(arr, low, high, criterion, ascending);
        quickSort(arr, low, pi - 1, criterion, ascending);
        quickSort(arr, pi + 1, high, criterion, ascending);
    }
}

void Sorting::merge(Fault arr[], int left, int mid, int right, SortCriterion criterion, bool ascending) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Fault* L = new Fault[n1];
    Fault* R = new Fault[n2];

    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {
        if (compare(L[i], R[j], criterion, ascending) || (!compare(R[j], L[i], criterion, ascending))) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    delete[] L;
    delete[] R;
}

// Manual MergeSort Implementation
void Sorting::mergeSort(Fault arr[], int left, int right, SortCriterion criterion, bool ascending) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid, criterion, ascending);
        mergeSort(arr, mid + 1, right, criterion, ascending);
        merge(arr, left, mid, right, criterion, ascending);
    }
}

// ============================================================
// BUBBLE SORT - O(n^2) Time, O(1) Space
// Repeatedly swaps adjacent elements if they are in wrong order.
// Each pass bubbles the largest unsorted element to its correct position.
// ============================================================
void Sorting::bubbleSort(Fault arr[], int n, SortCriterion criterion, bool ascending) {
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (!compare(arr[j], arr[j + 1], criterion, ascending)) {
                Fault temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// ============================================================
// SELECTION SORT - O(n^2) Time, O(1) Space
// Divides array into sorted and unsorted parts.
// Each pass selects the minimum from the unsorted part and
// places it at the end of the sorted part.
// ============================================================
void Sorting::selectionSort(Fault arr[], int n, SortCriterion criterion, bool ascending) {
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (compare(arr[j], arr[minIdx], criterion, ascending)) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            Fault temp = arr[i];
            arr[i] = arr[minIdx];
            arr[minIdx] = temp;
        }
    }
}

// ============================================================
// INSERTION SORT - O(n^2) Time avg, O(n) best case, O(1) Space
// Builds sorted array one element at a time.
// Each element is inserted into its correct position in the sorted portion.
// ============================================================
void Sorting::insertionSort(Fault arr[], int n, SortCriterion criterion, bool ascending) {
    for (int i = 1; i < n; ++i) {
        Fault key = arr[i];
        int j = i - 1;
        while (j >= 0 && !compare(arr[j], key, criterion, ascending) &&
               (compare(key, arr[j], criterion, ascending))) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

std::string Sorting::getSortAlgorithmName(int algo) {
    switch (algo) {
        case 1: return "Bubble Sort";
        case 2: return "Selection Sort";
        case 3: return "Insertion Sort";
        case 4: return "Quick Sort";
        case 5: return "Merge Sort";
        default: return "Unknown Algorithm";
    }
}
