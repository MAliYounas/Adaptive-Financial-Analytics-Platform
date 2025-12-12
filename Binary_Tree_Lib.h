#ifndef Binary_Tree_Lib_H
#define Binary_Tree_Lib_H

#include <iostream>
#include "Queue_Link_List_Implementation_Lib.h"  
using namespace std;

template <typename T>
class BinaryTree {
public:
    struct Node {
        T val;
        Node* left;
        Node* right;
        Node(const T& v) : val(v), left(nullptr), right(nullptr) {}
    };

private:
    Node* root;

    void inorder(Node* node) const {
        if (!node) return;
        inorder(node->left);
        cout << node->val << " ";
        inorder(node->right);
    }

    void preorder(Node* node) const {
        if (!node) return;
        cout << node->val << " ";
        preorder(node->left);
        preorder(node->right);
    }

    void postorder(Node* node) const {
        if (!node) return;
        postorder(node->left);
        postorder(node->right);
        cout << node->val << " ";
    }

    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    bool search(Node* node, const T& value) const {
        if (!node) return false;
        if (node->val == value) return true;
        return search(node->left, value) || search(node->right, value);
    }

    int height(Node* node) const {
        if (!node) return 0;
        return 1 + max(height(node->left), height(node->right));
    }

    int count_nodes(Node* node) const {
        if (!node) return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }

    int count_leaves(Node* node) const {
        if (!node) return 0;
        if (!node->left && !node->right) return 1;
        return count_leaves(node->left) + count_leaves(node->right);
    }

public:
    BinaryTree() : root(nullptr) {}
    ~BinaryTree() { clear(); }

    Node* get_root() const { return root; }

    void clear() {
        clear(root);
        root = nullptr;
    }

    void insert(const T& value) {
        Node* n = new Node(value);
        if (!root) {
            root = n;
            return;
        }

        Queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* cur = q.front();
            q.pop();

            if (!cur->left) {
                cur->left = n;
                return;
            } else q.push(cur->left);

            if (!cur->right) {
                cur->right = n;
                return;
            } else q.push(cur->right);
        }
    }

    bool search(const T& value) const { return search(root, value); }

    void inorder() const { inorder(root); cout << endl; }
    void preorder() const { preorder(root); cout << endl; }
    void postorder() const { postorder(root); cout << endl; }

    void level_order() const {
        if (!root) return;
        Queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* cur = q.front();
            q.pop();
            cout << cur->val << " ";
            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
        cout << endl;
    }

    int height() const { return height(root); }
    int size() const { return count_nodes(root); }
    int leaf_count() const { return count_leaves(root); }
};

#endif
