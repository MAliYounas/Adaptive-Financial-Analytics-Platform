#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "Vector.h"

using namespace std;

template<typename T>
class MinHeap {
private:
    Vector<T> heap;

    void heapify_up(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent] <= heap[index]) {
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
            int smallest = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < heap.size() && heap[left] < heap[smallest]) {
                smallest = left;
            }
            if (right < heap.size() && heap[right] < heap[smallest]) {
                smallest = right;
            }

            if (smallest == index) {
                break;
            }
            T temp = heap[index];
            heap[index] = heap[smallest];
            heap[smallest] = temp;
            index = smallest;
        }
    }

public:
    explicit MinHeap() {}

    MinHeap(const MinHeap& other) : heap(other.heap) {}

    MinHeap(MinHeap&& other) : heap(other.heap) {
        other.heap = Vector<T>();
    }

    MinHeap& operator=(const MinHeap& other) {
        if (this != &other) {
            heap = other.heap;
        }
        return *this;
    }

    MinHeap& operator=(MinHeap&& other) {
        if (this != &other) {
            heap = other.heap;
            other.heap = Vector<T>();
        }
        return *this;
    }

    ~MinHeap() {}

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

