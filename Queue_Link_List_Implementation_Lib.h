#ifndef Queue_Link_List_Implementation_Lib_H
#define Queue_Link_List_Implementation_Lib_H

#include <iostream>
using namespace std;

template <typename T>
class Queue
{
private:
    struct Node
    {
        T val;
        Node *next;
        Node(const T &v) : val(v), next(nullptr) {}
    };

    Node *frontNode;
    Node *rearNode;
    size_t sz;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), sz(0) {}
    ~Queue()
    {
        while (frontNode)
        {
            Node *temp = frontNode;
            frontNode = frontNode->next;
            delete temp;
        }
    }

    void push(const T &value)
    {
        Node *n = new Node(value);
        if (!rearNode)
        {
            frontNode = rearNode = n;
        }
        else
        {
            rearNode->next = n;
            rearNode = n;
        }
        ++sz;
    }

    bool pop()
    {
        if (!frontNode)
            return false;
        Node *temp = frontNode;
        frontNode = frontNode->next;
        if (!frontNode)
            rearNode = nullptr;
        delete temp;
        --sz;
        return true;
    }

    T &front()
    {
        if (!frontNode)
            throw out_of_range("Queue is empty");
        return frontNode->val;
    }

    T &back()
    {
        if (!rearNode)
            throw out_of_range("Queue is empty");
        return rearNode->val;
    }

    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }

    void print() const
    {
        Node *cur = frontNode;
        while (cur)
        {
            cout << cur->val << " -> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }
};

#endif
