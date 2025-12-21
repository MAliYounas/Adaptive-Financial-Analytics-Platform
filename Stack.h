#ifndef STACK_H
#define STACK_H

using namespace std;

template<typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {
        }
    };

    Node* top_;
    int size_;

    void clear_nodes() {
        while (top_) {
            Node* temp = top_;
            top_ = top_->next;
            delete temp;
        }
        size_ = 0;
    }

public:
    explicit Stack() : top_(nullptr), size_(0) {
    }

    Stack(const Stack& other) : top_(nullptr), size_(0) {
        if (other.top_) {
            Vector<T> vals;
            Node* cur = other.top_;
            while (cur) {
                vals.push_back(cur->data);
                cur = cur->next;
            }
            for (int i = vals.size() - 1; i >= 0; --i) {
                push(vals[i]);
            }
        }
    }

    Stack(Stack&& other) : top_(other.top_), size_(other.size_) {
        other.top_ = nullptr;
        other.size_ = 0;
    }

    Stack& operator=(const Stack& other) {
        if (this != &other) {
            clear_nodes();
            if (other.top_) {
                Vector<T> vals;
                Node* cur = other.top_;
                while (cur) {
                    vals.push_back(cur->data);
                    cur = cur->next;
                }
                for (int i = vals.size() - 1; i >= 0; --i) {
                    push(vals[i]);
                }
            }
        }
        return *this;
    }

    Stack& operator=(Stack&& other) {
        if (this != &other) {
            clear_nodes();
            top_ = other.top_;
            size_ = other.size_;
            other.top_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~Stack() {
        clear_nodes();
    }

    void push(const T& value) {
        Node* node = new Node(value);
        node->next = top_;
        top_ = node;
        ++size_;
    }

    void pop() {
        if (top_) {
            Node* temp = top_;
            top_ = top_->next;
            delete temp;
            --size_;
        }
    }

    T& top() {
        if (!top_) {
            throw "Stack is empty";
        }
        return top_->data;
    }

    bool empty() {
        return top_ == nullptr;
    }

    int size() {
        return size_;
    }
};

#endif









