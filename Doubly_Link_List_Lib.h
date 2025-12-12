#ifndef Doubly_Link_List_Lib_H
#define Doubly_Link_List_Lib_H

#include <iostream>
#include <vector>
using namespace std;

template <typename T>
class DoublyLinkedList
{
public:
    struct Node
    {
        T val;
        Node *prev;
        Node *next;
        Node(const T &v) : val(v), prev(nullptr), next(nullptr) {}
    };

private:
    Node *head;
    Node *tail;
    size_t sz;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), sz(0) {}
    ~DoublyLinkedList() { clear(); }

    void push_front(const T &value)
    {
        Node *n = new Node(value);
        if (!head)
            head = tail = n;
        else
        {
            n->next = head;
            head->prev = n;
            head = n;
        }
        ++sz;
    }

    void push_back(const T &value)
    {
        Node *n = new Node(value);
        if (!tail)
            head = tail = n;
        else
        {
            tail->next = n;
            n->prev = tail;
            tail = n;
        }
        ++sz;
    }

    bool pop_front()
    {
        if (!head)
            return false;
        Node *temp = head;
        head = head->next;
        if (head)
            head->prev = nullptr;
        else
            tail = nullptr;
        delete temp;
        --sz;
        return true;
    }

    bool pop_back()
    {
        if (!tail)
            return false;
        Node *temp = tail;
        tail = tail->prev;
        if (tail)
            tail->next = nullptr;
        else
            head = nullptr;
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

    void print_forward() const
    {
        Node *cur = head;
        while (cur)
        {
            cout << cur->val << " <-> ";
            cur = cur->next;
        }
        cout << "NULL\n";
    }

    void print_backward() const
    {
        Node *cur = tail;
        while (cur)
        {
            cout << cur->val << " <-> ";
            cur = cur->prev;
        }
        cout << "NULL\n";
    }
};

#endif
