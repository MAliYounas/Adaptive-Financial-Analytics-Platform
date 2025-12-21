#ifndef HASH_MAP_H
#define HASH_MAP_H

using namespace std;

template<typename Key, typename Value>
class HashMap {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;
        explicit Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };

    static const int DEFAULT_CAPACITY = 16;
    Node** buckets;
    int capacity_;
    int size_;

    int hash_function(const Key& key) {
        int hash = 0;
        const char* key_ptr = reinterpret_cast<const char*>(&key);
        int key_size = sizeof(Key);
        for (int i = 0; i < key_size; ++i) {
            hash = (hash * 31 + static_cast<unsigned char>(key_ptr[i])) % capacity_;
            if (hash < 0) {
                hash += capacity_;
            }
        }
        if (hash >= 0) {
            return hash;
        } else {
            return -hash;
        }
    }

    void rehash() {
        int old_capacity = capacity_;
        Node** old_buckets = buckets;
        capacity_ *= 2;
        buckets = new Node*[capacity_]();
        size_ = 0;

        for (int i = 0; i < old_capacity; ++i) {
            Node* current = old_buckets[i];
            while (current) {
                insert(current->key, current->value);
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] old_buckets;
    }

    void clear_buckets() {
        for (int i = 0; i < capacity_; ++i) {
            Node* current = buckets[i];
            while (current) {
                Node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] buckets;
    }

public:
    explicit HashMap(int initial_capacity = DEFAULT_CAPACITY)
        : capacity_(initial_capacity), size_(0) {
        buckets = new Node*[capacity_]();
        for (int i = 0; i < capacity_; ++i) {
            buckets[i] = nullptr;
        }
    }

    HashMap(const HashMap& other)
        : capacity_(other.capacity_), size_(0) {
        buckets = new Node*[capacity_]();
        for (int i = 0; i < capacity_; ++i) {
            buckets[i] = nullptr;
        }
        for (int i = 0; i < other.capacity_; ++i) {
            Node* current = other.buckets[i];
            while (current) {
                insert(current->key, current->value);
                current = current->next;
            }
        }
    }

    HashMap(HashMap&& other)
        : buckets(other.buckets), capacity_(other.capacity_), size_(other.size_) {
        other.buckets = nullptr;
        other.capacity_ = 0;
        other.size_ = 0;
    }

    HashMap& operator=(const HashMap& other) {
        if (this != &other) {
            clear_buckets();
            capacity_ = other.capacity_;
            size_ = 0;
            buckets = new Node*[capacity_]();
            for (int i = 0; i < capacity_; ++i) {
                buckets[i] = nullptr;
            }
            for (int i = 0; i < other.capacity_; ++i) {
                Node* current = other.buckets[i];
                while (current) {
                    insert(current->key, current->value);
                    current = current->next;
                }
            }
        }
        return *this;
    }

    HashMap& operator=(HashMap&& other) {
        if (this != &other) {
            clear_buckets();
            buckets = other.buckets;
            capacity_ = other.capacity_;
            size_ = other.size_;
            other.buckets = nullptr;
            other.capacity_ = 0;
            other.size_ = 0;
        }
        return *this;
    }

    ~HashMap() {
        clear_buckets();
    }

    void insert(const Key& key, const Value& value) {
        if (size_ >= capacity_ * 0.75) {
            rehash();
        }

        int index = hash_function(key);
        Node* current = buckets[index];

        while (current) {
            if (current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }

        Node* new_node = new Node(key, value);
        new_node->next = buckets[index];
        buckets[index] = new_node;
        ++size_;
    }

    bool contains(const Key& key) {
        int index = hash_function(key);
        Node* current = buckets[index];
        while (current) {
            if (current->key == key) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    Value& operator[](const Key& key) {
        int index = hash_function(key);
        Node* current = buckets[index];
        while (current) {
            if (current->key == key) {
                return current->value;
            }
            current = current->next;
        }

        insert(key, Value{});
        return buckets[hash_function(key)]->value;
    }

    void erase(const Key& key) {
        int index = hash_function(key);
        Node* current = buckets[index];
        Node* prev = nullptr;

        while (current) {
            if (current->key == key) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    buckets[index] = current->next;
                }
                delete current;
                --size_;
                return;
            }
            prev = current;
            current = current->next;
        }
    }

    int size() {
        return size_;
    }

    bool empty() {
        return size_ == 0;
    }
};

#endif

