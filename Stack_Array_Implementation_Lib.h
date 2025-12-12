#ifndef Stack_Array_Implementation_Lib_H
#define Stack_Array_Implementation_Lib_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class StackArray
{
private:
    T *arr;
    int capacity;
    int topIndex;

public:
    StackArray(int cap = 100) : capacity(cap), topIndex(-1)
    {
        arr = new T[capacity];
    }

    ~StackArray()
    {
        delete[] arr;
    }

    void push(const T &value)
    {
        if (topIndex >= capacity - 1)
            throw overflow_error("Stack Overflow");
        arr[++topIndex] = value;
    }

    bool pop()
    {
        if (topIndex < 0)
            return false;
        --topIndex;
        return true;
    }

    T &top()
    {
        if (topIndex < 0)
            throw out_of_range("Stack is empty");
        return arr[topIndex];
    }

    bool empty() const { return topIndex < 0; }
    int size() const { return topIndex + 1; }

    void print() const
    {
        for (int i = topIndex; i >= 0; --i)
            cout << arr[i] << " -> ";
        cout << "NULL\n";
    }
};

#endif
