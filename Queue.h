#ifndef QUEUE_H
#define QUEUE_H

using namespace std;

template<typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* front_;
    Node* rear_;
    int size_;

    void clear_nodes() {
        while (front_) {
            Node* temp = front_;
            front_ = front_->next;
            delete temp;
        }
        rear_ = nullptr;
        size_ = 0;
    }

public:
    explicit Queue() : front_(nullptr), rear_(nullptr), size_(0) {}

    Queue(const Queue& other) : front_(nullptr), rear_(nullptr), size_(0) {
        Node* current = other.front_;
        while (current) {
            enqueue(current->data);
            current = current->next;
        }
    }

    Queue(Queue&& other)
        : front_(other.front_), rear_(other.rear_), size_(other.size_) {
        other.front_ = other.rear_ = nullptr;
        other.size_ = 0;
    }

    Queue& operator=(const Queue& other) {
        if (this != &other) {
            clear_nodes();
            Node* current = other.front_;
            while (current) {
                enqueue(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    Queue& operator=(Queue&& other) {
        if (this != &other) {
            clear_nodes();
            front_ = other.front_;
            rear_ = other.rear_;
            size_ = other.size_;
            other.front_ = other.rear_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~Queue() {
        clear_nodes();
    }

    void enqueue(const T& value) {
        Node* new_node = new Node(value);
        if (!rear_) {
            front_ = rear_ = new_node;
        } else {
            rear_->next = new_node;
            rear_ = new_node;
        }
        ++size_;
    }

    void dequeue() {
        if (front_) {
            Node* temp = front_;
            front_ = front_->next;
            if (!front_) {
                rear_ = nullptr;
            }
            delete temp;
            --size_;
        }
    }

    T& front() {
        if (!front_) {
            throw "Queue is empty";
        }
        return front_->data;
    }

    bool empty() {
        return front_ == nullptr;
    }

    int size() {
        return size_;
    }
};

#endif

