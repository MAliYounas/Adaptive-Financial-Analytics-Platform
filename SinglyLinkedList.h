#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

using namespace std;

template<typename T>
class SinglyLinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val) : data(val), next(nullptr) {}
    };

    Node* head;
    int size_;

    void clear_nodes() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        size_ = 0;
    }

public:
    explicit SinglyLinkedList() : head(nullptr), size_(0) {}

    SinglyLinkedList(const SinglyLinkedList& other) : head(nullptr), size_(0) {
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }

    SinglyLinkedList(SinglyLinkedList&& other)
        : head(other.head), size_(other.size_) {
        other.head = nullptr;
        other.size_ = 0;
    }

    SinglyLinkedList& operator=(const SinglyLinkedList& other) {
        if (this != &other) {
            clear_nodes();
            Node* current = other.head;
            while (current) {
                push_back(current->data);
                current = current->next;
            }
        }
        return *this;
    }

    SinglyLinkedList& operator=(SinglyLinkedList&& other) {
        if (this != &other) {
            clear_nodes();
            head = other.head;
            size_ = other.size_;
            other.head = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~SinglyLinkedList() {
        clear_nodes();
    }

    void push_front(const T& value) {
        Node* new_node = new Node(value);
        new_node->next = head;
        head = new_node;
        ++size_;
    }

    void push_back(const T& value) {
        Node* new_node = new Node(value);
        if (!head) {
            head = new_node;
        } else {
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = new_node;
        }
        ++size_;
    }

    void pop_front() {
        if (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
            --size_;
        }
    }

    bool empty() {
        return head == nullptr;
    }

    int size() {
        return size_;
    }

    T& front() {
        if (!head) {
            throw "List is empty";
        }
        return head->data;
    }
};

#endif

