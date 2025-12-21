#ifndef VECTOR_H
#define VECTOR_H

using namespace std;

template<typename T>
class Vector {
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
    explicit Vector(int initial_capacity = 10)
        : size_(0), capacity_(initial_capacity) {
        data = new T[capacity_];
    }

    Vector(const Vector& other)
        : size_(other.size_), capacity_(other.capacity_) {
        data = new T[capacity_];
        for (int i = 0; i < size_; ++i) {
            data[i] = other.data[i];
        }
    }

    Vector(Vector&& other)
        : data(other.data), size_(other.size_), capacity_(other.capacity_) {
        other.data = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    Vector& operator=(const Vector& other) {
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

    Vector& operator=(Vector&& other) {
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

    ~Vector() {
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
            throw "Vector index out of range";
        }
        return data[index];
    }


    T& back() {
        if (size_ == 0) {
            throw "Vector is empty";
        }
        return data[size_ - 1];
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

    void erase(int index) {
        if (index < 0 || index >= size_) {
            return;
        }
        for (int i = index; i < size_ - 1; ++i) {
            data[i] = data[i + 1];
        }
        --size_;
    }
};

#endif

