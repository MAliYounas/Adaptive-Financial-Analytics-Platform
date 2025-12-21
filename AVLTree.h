#ifndef AVL_TREE_H
#define AVL_TREE_H

using namespace std;

template<typename T>
class AVLTree {
private:
    struct Node {
        T data;
        Node* left;
        Node* right;
        int height;

        explicit Node(T val)
            : data(val), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;

    int get_height(Node* node) {
        if (node) {
            return node->height;
        } else {
            return 0;
        }
    }

    int get_balance(Node* node) {
        if (node) {
            return get_height(node->left) - get_height(node->right);
        } else {
            return 0;
        }
    }

    int max(int a, int b) {
        if (a > b) {
            return a;
        } else {
            return b;
        }
    }

    Node* right_rotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(get_height(y->left), get_height(y->right)) + 1;
        x->height = max(get_height(x->left), get_height(x->right)) + 1;

        return x;
    }

    Node* left_rotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(get_height(x->left), get_height(x->right)) + 1;
        y->height = max(get_height(y->left), get_height(y->right)) + 1;

        return y;
    }

    Node* insert_node(Node* node, T& value) {
        if (!node) {
            return new Node(value);
        }

        if (value < node->data) {
            node->left = insert_node(node->left, value);
        } else if (value > node->data) {
            node->right = insert_node(node->right, value);
        } else {
            return node;
        }

        node->height = 1 + max(get_height(node->left),
                                    get_height(node->right));

        int balance = get_balance(node);

        if (balance > 1 && value > node->left->data) {
            node->left = left_rotate(node->left);
            return right_rotate(node);
        }

        if (balance < -1 && value < node->right->data) {
            node->right = right_rotate(node->right);
            return left_rotate(node);
        }

        if (balance > 1 && value < node->left->data) {
            return right_rotate(node);
        }

        if (balance < -1 && value > node->right->data) {
            return left_rotate(node);
        }

        return node;
    }

    Node* copy_tree(Node* node) {
        if (!node) {
            return nullptr;
        }
        Node* new_node = new Node(node->data);
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        new_node->height = node->height;
        return new_node;
    }

    void clear_tree(Node* node) {
        if (node) {
            clear_tree(node->left);
            clear_tree(node->right);
            delete node;
        }
    }

    bool search_node(Node* node, T& value) {
        if (!node) {
            return false;
        }
        if (value == node->data) {
            return true;
        }
        if (value < node->data) {
            return search_node(node->left, value);
        }
        return search_node(node->right, value);
    }

public:
    explicit AVLTree() : root(nullptr) {}

    AVLTree(const AVLTree& other) : root(nullptr) {
        root = copy_tree(other.root);
    }

    AVLTree(AVLTree&& other) : root(other.root) {
        other.root = nullptr;
    }

    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clear_tree(root);
            root = copy_tree(other.root);
        }
        return *this;
    }

    AVLTree& operator=(AVLTree&& other) {
        if (this != &other) {
            clear_tree(root);
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    ~AVLTree() {
        clear_tree(root);
    }

    void insert(T& value) {
        root = insert_node(root, value);
    }

    bool search(T& value) {
        return search_node(root, value);
    }

    bool empty() {
        return root == nullptr;
    }
};

#endif

