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
        iterator &operator++(){ _node = _node->_next; return *(this); }
        iterator operator++(int) { this->_node = _node->_next; return *(this); }
        iterator &operator--() { _node = _node->_prev; return *(this); }
        iterator operator--(int) { this->_node = _node->_prev; return *(this); }
        iterator &operator=(const iterator &i) { _node = i._node; return *(this); }

        bool operator!=(const iterator &i) const { return (this->_node != i._node); }
        bool operator==(const iterator &i) const { return (this->_node == i._node); }

      private:
        DListNode<T> *_node;
    };

    // TODO: implement these functions
    iterator begin() const { return iterator(_head); }      //
    iterator end() const { return iterator(_head->_prev); } // which's->prev == last node
    iterator back() const { return iterator(_head->_prev->_prev); }

    bool empty() const
    {
        if (size() == 0) return true;
        return false;
    }
    size_t size() const 
    {
        size_t size = 0;

        for (iterator ptr = begin(); ptr != end(); ++ptr)
            size++;

        return size;
    }

    void push_back(const T &x)
    {
        
        if (empty())
        {
            DListNode<T> *t = new DListNode<T>(x, _head, _head);
            _head->_next = _head->_prev = t;
            t->_prev = t->_next = _head;
            _head = t;
            return;
        }
        // Otherwise, create a new list "t"
        // Original tail : dummy->prev
        // Let t->prev be dummy->prev, t->next be dummy
        DListNode<T> *t = new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
        //                                       ^ last ^             ^ dummy ^
        ((_head->_prev)->_prev)->_next = t;
        (_head->_prev)->_prev = t;
        // ^ dummy node
        //_size++;

        //iterator ed = end();
        //--end();
        //insert(x, ed);
    }

    void pop_front()
    {
        if (empty())
            return;
        DListNode<T> *t = _head->_next;
        t->_prev = _head->_prev; // link new "first" to dummy node
        (_head->_prev)->_next = t; // dummy->_next = t
        delete _head;
        _head = t;
        //_size--;
    }

    void pop_back()
    {
        if (empty())
            return;
        DListNode<T> *t = _head->_prev->_prev; // set t as the last element
        (_head->_prev)->_prev = t->_prev;       // link dummy node to "new" last node
        (t->_prev)->_next = _head->_prev;       // link "new" last ode to dummy node
        delete t;                             // delete "old" last node
        //_size--;
    }

    // erase by iterator
    // return false if nothing to erase
    bool erase(iterator pos)
    {
        if (empty())
            return false;
        
        (pos._node->_prev)->_next = pos._node->_next;
        (pos._node->_next)->_prev = pos._node->_prev;
        if (pos == begin() && size() == 1)
        {
            pos._node->_next = pos._node->_prev = _head->_prev;
            _head =  _head->_prev;
        }
        else if (pos == begin())
        {
            delete pos._node;
            _head =  pos._node->_next;
        }
        else
            delete pos._node;
        //_size--;
        return true;
    }

    // erase by data? -> Yes
    bool erase(const T &x)
    {
        if (empty()) return false;

        for (DList<T>::iterator it = begin(); it != end(); ++it)
        {
            if (*it == x)
                return erase(it);
        }
        return false;
    }

    // delete all the dlist nodes but dummy node
    void clear()
    {
        if (empty()) return;
        DListNode<T> *dummy = _head->_prev;
        DListNode<T> *t = _head;

        
        //size_t cnt = 0;
        // traversal... form _head
        while (t != dummy)
        {
            DListNode<T> *tmp = t;
            t = t->_next;
            delete tmp;
            //cnt++;
        }

        //cerr << cnt << endl;
        // reset _head to dummy node
        _head = dummy;
        _head->_prev = _head->_next = dummy;
        //_size = 0; // reset list size to 0
    }

    iterator find(const T &x) const
    {
        for (DList<T>::iterator it = begin(); it != end(); ++it)
        {
            if (*it == x)
                return it;
        }
        return end();
    }

    void sort() const
    {
        //TODO
        //先用bubble sort
        iterator ed = end();

        //size_t cnt = 0;
        
        for (iterator it = begin(); it != ed; ++it)
        {
            for(iterator li = it; li != ed; ++li)
            {
                if(*it > *li)
                {
                    //cerr << "it:" << *it << ","
                    //     << "li:" << *li << endl;
                    swap(it, li);
                    //cnt++;
                }

            }
        }
        //cerr << "swap:" << cnt << endl;
        //--ed;
        //quickSort(begin(), ed);

        _isSorted = true;
    }

    void quickSort(const iterator &front, const iterator &term) const
    {
        if(front != term && front != term )//&& front != term++)
        {
            iterator pivot = partition(front, term);
            quickSort(front, --pivot);
            quickSort(++pivot, term);
        }
    }

    // partition for quick sort
    iterator partition(const iterator &front, const iterator &term) const
    {
        iterator pivot = term;
        iterator i = front;
        --i;
        for (iterator j = front; j != end(); ++j)
        {
            if(*j < *pivot)
            {
                ++i;
                swap(i, j);
            }
        }
        ++i;
        swap(i, pivot);
        return i;
    }
    /*
    DListNode<T> *split(DListNode<T> *head)
    {
        DListNode<T> *top = head, *down = head;
        while(top->_next != _head->_prev && top->_next->_next != _head->_prev)
        {
            top = top->_next->_next;
            down = down->_next;
        }
        DListNode<T> *tmp = down->_next;
        down->_next = _head->_prev;
        return tmp;
    }

    DListNode<T> *merge(DListNode<T> *start, DListNode<T> *term) 
    {
        if(start == _head->_prev)
            return term;
        if(term == _head->_prev)
            return start;

        if(start->_data < term->_data)
        {
            start->_next = merge(start->_next, term);
            (start->_next)->_prev = start;
            start->_prev = _head->_prev;
            return start;
        }
        else
        {
            term->_next = merge(start, term->_next);
            (term->_next)->_prev = term;
            term->_prev = _head->_prev;
            return term;
        }
    }

    DListNode<T> *mergeSort(DListNode<T> *&head)
    {
        if(head == _head->_prev || head->_next == _head->_prev)
            return head;

        DListNode<T> *term = split(head);
        head = mergeSort(head);
        term = mergeSort(term);
        return merge(head, term);
    }
    */

    // insert element after iterator pos
    void insert(const T &x, iterator pos)
    {
        if (empty())
        {
            DListNode<T> *t = new DListNode<T>(x, _head, _head);
            _head->_next = _head->_prev = t;
            t->_prev = t->_next = _head;
            _head = t;
            return;
        }

        DListNode<T> *t = new DListNode<T>(x, pos._node, pos._node->_next);
        pos._node->_next = t;
        (pos._node->_next)->_prev = t;
        //_size++
    }

  private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    DListNode<T> *_head;    // = dummy node if list is empty
    mutable bool _isSorted; // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] helper functions; called by public member functions
    void swap(iterator &i, iterator &j) const
    {
        T tmp = *j;
        *j = *i;
        *i = tmp;
    }
};

#endif // DLIST_H
