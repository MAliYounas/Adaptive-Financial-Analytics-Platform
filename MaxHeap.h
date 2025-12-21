#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "Vector.h"

using namespace std;

template<typename T>
class MaxHeap {
private:
    Vector<T> heap;

    void heapify_up(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent] >= heap[index]) {
                break;
            }
            T temp = heap[parent];
            heap[parent] = heap[index];
            heap[index] = temp;
            index = parent;
        }
    }

    void heapify_down(int index) {
        while (true) {
            int largest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < heap.size() && heap[left] > heap[largest]) {
                largest = left;
            }
            if (right < heap.size() && heap[right] > heap[largest]) {
                largest = right;
            }

            if (largest == index) {
                break;
            }
            T temp = heap[index];
            heap[index] = heap[largest];
            heap[largest] = temp;
            index = largest;
        }
    }

public:
    explicit MaxHeap() {}

    MaxHeap(const MaxHeap& other) : heap(other.heap) {}

    MaxHeap(MaxHeap&& other) : heap(other.heap) {
        other.heap = Vector<T>();
    }

    MaxHeap& operator=(const MaxHeap& other) {
        if (this != &other) {
            heap = other.heap;
        }
        return *this;
    }

    MaxHeap& operator=(MaxHeap&& other) {
        if (this != &other) {
            heap = other.heap;
            other.heap = Vector<T>();
        }
        return *this;
    }

    ~MaxHeap() {}

    void push(const T& value) {
        heap.push_back(value);
        heapify_up(heap.size() - 1);
    }

    void pop() {
        if (heap.empty()) {
            return;
        }
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            heapify_down(0);
        }
    }

    T& top() {
        if (heap.empty()) {
            throw "Heap is empty";
        }
        return heap[0];
    }

    bool empty() {
        return heap.empty();
    }

    int size() {
        return heap.size();
    }
};

#endif

