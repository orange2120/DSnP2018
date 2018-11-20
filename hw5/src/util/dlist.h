/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T>
class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
    friend class DList<T>;
    friend class DList<T>::iterator;

    DListNode(const T &d, DListNode<T> *p = 0, DListNode<T> *n = 0) : _data(d), _prev(p), _next(n) {}

    // [NOTE] DO NOT ADD or REMOVE any data member
    T _data;
    DListNode<T> *_prev;
    DListNode<T> *_next;
};

template <class T>
class DList
{
  public:
    // TODO: decide the initial value for _isSorted
    DList()
    {
        _head = new DListNode<T>(T());
        _head->_prev = _head->_next = _head; // _head is a dummy node
    }
    ~DList()
    {
        clear();
        delete _head;
    }

    // DO NOT add any more data member or function for class iterator
    class iterator
    {
        friend class DList;

      public:
        iterator(DListNode<T> *n = 0) : _node(n) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        // TODO: implement these overloaded operators
        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++()
        {
            _node = _node->_next;
            return *(this);
        }
        iterator operator++(int) { return iterator(_node->_next); }
        iterator &operator--()
        {
            _node = _node->_prev;
            return *(this);
        }
        iterator operator--(int) { return iterator(_node->_prev); }
        iterator &operator=(const iterator &i)
        {
            _node = i._node;
            return *(this);
        }

        bool operator!=(const iterator &i) const { return this->_node != i._node; }
        bool operator==(const iterator &i) const
        {
            return this->_node == i._node;
        }

      private:
        DListNode<T> *_node;
    };

    // TODO: implement these functions
    iterator begin() const { return iterator(_head); }      //
    iterator end() const { return iterator(_head->_prev); } // who's->prev == last node
    bool empty() const
    {
        // If _dummy point to itself
        if (_size == 0)
            return true;
        else
            return false;
    }
    size_t size() const { return _size; }

    void push_back(const T &x)
    {
        // If list is empty, create a list point to dummy node.
        if (empty())
            DListNode<T> t = new DListNode<T>(x, _head, _head);
        // Otherwise, create a new list "t"
        //
        // Original tail : dummy->prev
        // Let t->prev be dummy->prev, t->next be dummy
        else
        {
            DListNode<T> *t = new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
            _head->_prev->_prev->_next = t;
            _head->_prev->_prev = t;
        }
        _size++;
    }
    void pop_front()
    {
        if (empty())
            return;
        ListNode<T> *t = _head->_next;
        delete _head;
        _head->_next->_prev = t;
        _head = t;
        _size--;
    }
    void pop_back()
    {
        if (empty())
            return;
        ListNode<T> *t = _head->_next->_prev; // set t as the last element
        _head->_next->_next = t->_prev;       // link dummy node to "new" last node
        t->_prev->_next = _head->_next;       // link "new" last ode to dummy node
        delete t;                             // delete "old" last node
        _size--;
    }

    // return false if nothing to erase
    bool erase(iterator pos)
    {
        if (empty())
            return false;
        pos._node->_prev->_next = pos._node->_next;
        pos._node->_next->_prev = pos._node->_prev;
        delete pos._node;
        delete pos;
        _size--;
        return true;
    }
    // erase by data?
    bool erase(const T &x)
    {
        if (empty())
            return false;
        ListNode<T> *t = _head;
        // traversal...
        while (t != _head->_next)
        {
            if (t->_data == x)
            {
                t->_prev->_next = t->_next;
                t->_next->_prev = t->_prev;
                delete t;
                _size--;
                return true;
            }
            t = t->_next;
        }
        return false;
    }

    void clear()
    {
        if (empty())
            return;
        DListNode<T> *t = _head;
        // traversal...
        while (t != _head->_next)
        {
            DListNode<T> *tmp = t;
            t = t->_next;
            delete tmp;
        }
        _head->_prev = _head->_next = _head;
        _head->_next = _head;
        _size = 0; // reset list size to 0
    }              // delete all nodes except for the dummy node

    void sort() const
    {
        //TODO
    }

  private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    DListNode<T> *_head;    // = dummy node if list is empty
    mutable bool _isSorted; // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
