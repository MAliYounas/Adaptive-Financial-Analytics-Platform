#ifndef Stack_link_List_Implementation_Lib_H
#define Stack_link_List_Implementation_Lib_H

#include <iostream>
#include <stdexcept>
using namespace std;

template <typename T>
class Stack
{
private:
    struct Node
    {
        T val;
        Node *next;
        Node(const T &v) : val(v), next(nullptr) {}
    };

    Node *topNode;
    size_t sz;

public:
    Stack() : topNode(nullptr), sz(0) {}
    ~Stack()
    {
        while (topNode)
        {
            Node *temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    void push(const T &value)
    {
        Node *n = new Node(value);
        n->next = topNode;
        topNode = n;
        ++sz;
    }

    void pop()
    {
        if (!topNode)
            throw out_of_range("Stack is empty");
        Node *temp = topNode;
        topNode = topNode->next;
        delete temp;
        --sz;
    }

    T &top()
    {
        if (!topNode)
            throw out_of_range("Stack is empty");
        return topNode->val;
    }

    const T &top() const
    {
        if (!topNode)
            throw out_of_range("Stack is empty");
        return topNode->val;
    }

    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }

    void print() const
    {
        Node *cur = topNode;
        while (cur)
        {
            cout << cur->val << " -> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }
};

#endif
