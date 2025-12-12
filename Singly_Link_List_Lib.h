#ifndef Singly_Link_List_Lib_H
#define Singly_Link_List_Lib_H

#include <iostream>
#include <vector>
using namespace std;

template <typename T>
class SinglyLinkedList
{
public:
    struct Node
    {
        T val;
        Node *next;
        Node(const T &v) : val(v), next(nullptr) {}
    };

private:
    Node *head;
    size_t sz;

public:
    SinglyLinkedList() : head(nullptr), sz(0) {}
    ~SinglyLinkedList() { clear(); }

    void push_front(const T &value)
    {
        Node *n = new Node(value);
        n->next = head;
        head = n;
        ++sz;
    }

    void push_back(const T &value)
    {
        Node *n = new Node(value);
        if (!head)
            head = n;
        else
        {
            Node *cur = head;
            while (cur->next)
                cur = cur->next;
            cur->next = n;
        }
        ++sz;
    }

    bool pop_front()
    {
        if (!head)
            return false;
        Node *temp = head;
        head = head->next;
        delete temp;
        --sz;
        return true;
    }

    bool contains(const T &value) const
    {
        Node *cur = head;
        while (cur)
        {
            if (cur->val == value)
                return true;
            cur = cur->next;
        }
        return false;
    }

    size_t size() const { return sz; }
    bool empty() const { return sz == 0; }

    void clear()
    {
        while (head)
            pop_front();
    }

    vector<T> to_vector() const
    {
        vector<T> out;
        Node *cur = head;
        while (cur)
        {
            out.push_back(cur->val);
            cur = cur->next;
        }
        return out;
    }

    void print() const
    {
        Node *cur = head;
        while (cur)
        {
            cout << cur->val << " -> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }
};

#endif
