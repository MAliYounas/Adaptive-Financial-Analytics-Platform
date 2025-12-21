#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

using namespace std;

template<typename T>
class DoublyLinkedList {
private:
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& val) : data(val), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size_;

    void clear_nodes() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        head = tail = nullptr;
        size_ = 0;
    }

public:
    explicit DoublyLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

    DoublyLinkedList(const DoublyLinkedList& other)
        : head(nullptr), tail(nullptr), size_(0) {
        Node* current = other.head;
        while (current) {
            push_back(current->data);
            current = current->next;
        }
    }

    DoublyLinkedList(DoublyLinkedList&& other)
        : head(other.head), tail(other.tail), size_(other.size_) {
        other.head = other.tail = nullptr;
        other.size_ = 0;
    }

    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
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

    DoublyLinkedList& operator=(DoublyLinkedList&& other) {
        if (this != &other) {
            clear_nodes();
            head = other.head;
            tail = other.tail;
            size_ = other.size_;
            other.head = other.tail = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~DoublyLinkedList() {
        clear_nodes();
    }

    void push_front(const T& value) {
        Node* new_node = new Node(value);
        if (!head) {
            head = tail = new_node;
        } else {
            new_node->next = head;
            head->prev = new_node;
            head = new_node;
        }
        ++size_;
    }

    void push_back(const T& value) {
        Node* new_node = new Node(value);
        if (!tail) {
            head = tail = new_node;
        } else {
            tail->next = new_node;
            new_node->prev = tail;
            tail = new_node;
        }
        ++size_;
    }

    void pop_front() {
        if (head) {
            Node* temp = head;
            head = head->next;
            if (head) {
                head->prev = nullptr;
            } else {
                tail = nullptr;
            }
            delete temp;
            --size_;
        }
    }

    void pop_back() {
        if (tail) {
            Node* temp = tail;
            tail = tail->prev;
            if (tail) {
                tail->next = nullptr;
            } else {
                head = nullptr;
            }
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

    T& back() {
        if (!tail) {
            throw "List is empty";
        }
        return tail->data;
    }


    Vector<T> to_vector() {
        Vector<T> result;
        Node* current = head;
        while (current) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }


    T& at(int index) {
        if (index < 0 || index >= size_) {
            throw "Index out of range";
        }
        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }
};

#endif

