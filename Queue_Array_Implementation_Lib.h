#ifndef Queue_Array_Implementation_Lib_H
#define Queue_Array_Implementation_Lib_H

#include <iostream>
using namespace std;

template<typename T>
class QueueArray {
private:
    T* arr;
    int capacity;
    int frontIndex;
    int rearIndex;
    int cnt;

public:
    QueueArray(int cap = 100) : capacity(cap), frontIndex(0), rearIndex(0), cnt(0) {
        arr = new T[capacity];
    }

    ~QueueArray() {
        delete[] arr;
    }

    bool push(const T& value) {
        if (cnt == capacity) return false; // full
        arr[rearIndex] = value;
        rearIndex = (rearIndex + 1) % capacity;
        ++cnt;
        return true;
    }

    bool pop() {
        if (cnt == 0) return false; // empty
        frontIndex = (frontIndex + 1) % capacity;
        --cnt;
        return true;
    }

    T& front() {
        if (cnt == 0) throw out_of_range("Queue is empty");
        return arr[frontIndex];
    }

    T& back() {
        if (cnt == 0) throw out_of_range("Queue is empty");
        int idx = (rearIndex - 1 + capacity) % capacity;
        return arr[idx];
    }

    bool empty() const { return cnt == 0; }
    bool full() const { return cnt == capacity; }
    int size() const { return cnt; }

    void print() const {
        int idx = frontIndex;
        for (int i = 0; i < cnt; ++i) {
            cout << arr[idx] << " -> ";
            idx = (idx + 1) % capacity;
        }
        cout << "NULL\n";
    }
};

#endif
