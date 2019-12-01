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

        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++(){ _node = _node->_next; return *(this); }
        iterator operator++(int) { iterator t = *(this); ++*(this); return t; }
        iterator &operator--() { _node = _node->_prev; return *(this); }
        iterator operator--(int) { iterator t = *(this); --*(this); return t; }
        iterator &operator=(const iterator &i) { _node = i._node; return *(this); }

        bool operator!=(const iterator &i) const { return (this->_node != i._node); }
        bool operator==(const iterator &i) const { return (this->_node == i._node); }

      private:
        DListNode<T> *_node;
    };

    iterator begin() const { return iterator(_head); }      //
    iterator end() const { return iterator(_head->_prev); } // which's->prev == last node
    iterator back() const { return iterator(_head->_prev->_prev); }

    bool empty() const
    {
        if (_head->_prev == _head) return true;
        return false;
    }
    size_t size() const 
    {
        size_t size = 0;
        DListNode<T> *t = _head;
        while(t != _head->_prev)
        {
            size++;
            t = t->_next;
        }
        return size;
    }

    void push_back(const T &x)
    {
        // if list is empty, create a new list at first
        if (_head->_prev == _head)
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
        ((_head->_prev)->_prev)->_next = t;
        (_head->_prev)->_prev = t;
        _isSorted = false;
        //_size++;
    }

    void pop_front()
    {
        if (_head->_prev == _head) return;
        if(_head->_next->_next == _head)
        {
            DListNode<T> *t = _head;
            _head = _head->_prev;
            _head->_prev = _head->_next = _head;
            delete t;
            return;
        }
        DListNode<T> *t = _head->_next;
        t->_prev = _head->_prev; // link new "first" to dummy node
        (_head->_prev)->_next = t; // dummy->_next = t
        delete _head;
        _head = t;
        //_size--;
    }

    void pop_back()
    {
        // reach head
        if (_head->_prev == _head) return;
        // exist only one node => delete and set _head to dummy node
        if(_head->_next->_next == _head)
        {
            DListNode<T> *t = _head;
            _head = _head->_prev;
            _head->_prev = _head->_next = _head;
            delete t;
            return;
        }
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
        if (_head->_prev == _head) return false;
        
        (pos._node->_prev)->_next = pos._node->_next;
        (pos._node->_next)->_prev = pos._node->_prev;
        
        delete pos._node;
        // exist only one node => delete and set _head to dummy node
        if (pos._node == _head && _head->_next->_next == _head)
        {
            pos._node->_next = pos._node->_prev = _head->_prev;
            _head =  _head->_prev;
        }
        // target is _head
        else if (pos._node == _head)
        {
            _head =  pos._node->_next;
        }
        return true;
    }

    // erase by data? -> Yes
    bool erase(const T &x)
    {
        if (_head->_prev == _head) return false;
        iterator ed = end();
        for (iterator it = begin(); it != ed; ++it)
        {
            if (*it == x)
                return erase(it);
        }
        return false;
    }

    // delete all the dlist nodes but dummy node
    void clear()
    {
        if (_head->_prev == _head) return;
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
        _isSorted = false;
        //_size = 0; // reset list size to 0
    }

    iterator find(const T &x) const
    {
        iterator ed = end();
        for (iterator it = begin(); it != ed; ++it)
        {
            if (*it == x)
                return it;
        }
        return ed;
    }

    void sort() const
    {
        if(_isSorted) return;
        // bubble sort
        //size_t cnt = 0;
        /*
        iterator ed = end();
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
        */
        //cerr << "swap:" << cnt << endl;
        quickSort(_head, _head->_prev->_prev);

        _isSorted = true;
    }

    void quickSort(DListNode<T> *front, DListNode<T> *back) const
    {
        if (front != _head->_prev && front != back && front != back->_next)
        {
            DListNode<T> *pivot = partition(front, back);
            quickSort(front, pivot->_prev);
            quickSort(pivot->_next, back);
        }
    }

    // partition for quick sort
    DListNode<T> *partition(DListNode<T> *front, DListNode<T> *back) const
    {
        T t = back->_data;
        DListNode<T> *i = front->_prev;

        for (DListNode<T> *j = front; j != back;j = j->_next)
        {
            if(j->_data < t)
            {
                if(i == _head->_prev)
                    i = front;
                else
                    i = i->_next;
                
                T tmp = i->_data;
                i->_data = j->_data;
                j->_data = tmp;
                //cerr << "i:" << i->_data << ",j:" << j->_data << endl;
            }
        }
        if(i == _head->_prev)
            i = front;
        else
            i = i->_next;
            
        T ttmp = i->_data;
        i -> _data = back -> _data;
        back->_data = ttmp;
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
        _isSorted = false;
        if (_head->_prev == _head)
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
    mutable bool _isSorted = false; // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
