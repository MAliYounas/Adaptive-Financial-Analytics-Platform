#ifndef ARRAY_H
#define ARRAY_H

using namespace std;

template<typename T>
class Array {
private:
    T* data;
    int size_;
    int capacity_;

    void resize(int new_capacity) {
        T* new_data = new T[new_capacity];
        for (int i = 0; i < size_; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity_ = new_capacity;
    }

public:
    explicit Array(int initial_capacity = 10)
        : size_(0), capacity_(initial_capacity) {
        data = new T[capacity_];
    }

    Array(const Array& other)
        : size_(other.size_), capacity_(other.capacity_) {
        data = new T[capacity_];
        for (int i = 0; i < size_; ++i) {
            data[i] = other.data[i];
        }
    }

    Array(Array&& other)
        : data(other.data), size_(other.size_), capacity_(other.capacity_) {
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Array& operator=(const Array& other) {
        if (this != &other) {
            delete[] data;
            size_ = other.size_;
            capacity_ = other.capacity_;
            data = new T[capacity_];
            for (int i = 0; i < size_; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    Array& operator=(Array&& other) {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size_ = other.size_;
            capacity_ = other.capacity_;
            other.data = nullptr;
            other.size_ = 0;
            other.capacity_ = 0;
        }
        return *this;
    }

    ~Array() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (size_ >= capacity_) {
            resize(capacity_ * 2);
        }
        data[size_++] = value;
    }

    void pop_back() {
        if (size_ > 0) {
            --size_;
        }
    }

    T& operator[](int index) {
        if (index < 0 || index >= size_) {
            throw "Array index out of range";
        }
        return data[index];
    }

    int size() {
        return size_;
    }
    bool empty() {
        return size_ == 0;
    }

    void clear() {
        size_ = 0;
    }
};

#endif

