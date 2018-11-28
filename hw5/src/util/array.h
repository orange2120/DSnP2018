/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode XD
//
template <class T>
class Array
{
  public:
    // TODO: decide the initial value for _isSorted
    Array() : _data(0), _size(0), _capacity(0) {}
    ~Array() { delete[] _data; }

#define T_SIZE sizeof(T)

    // DO NOT add any more data member or function for class iterator
    class iterator
    {
        friend class Array;

      public:
        iterator(T *n = 0) : _node(n) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T &operator*() const { return *(this->_node); }
        T &operator*() { return *(this->_node); }
        iterator &operator++() { _node++; return *(this); }
        iterator operator++(int) { return iterator(_node++); }
        iterator &operator--() { _node--; return *(this); }
        iterator operator--(int) { return iterator(_node--); }
        iterator operator+(int i) const { return iterator(_node + i); }
        iterator &operator+=(int i) { this->_node = _node + i; return (*this); }
        iterator &operator=(const iterator &i) { this->_node = i._node; return (*this); }

        bool operator!=(const iterator &i) const { return (this->_node != i._node); }
        bool operator==(const iterator &i) const { return (this->_node == i._node); }

      private:
        T *_node;
    };

    iterator begin() const 
    { 
        if(_size == 0) return 0;
         return iterator(&_data[0]); 
    }
    
    iterator end() const 
    { 
        if(_size == 0) return 0;
        return iterator(&_data[_size - 1]+1);
    }

    bool empty() const 
    { 
        if(_size == 0) return true;
        return false;
    }

    size_t size() const { return _size; }

    T &operator[](size_t i) { return _data[i]; }
    const T &operator[](size_t i) const { return _data[i]; }

    void push_back(const T &x)
    {
        if (_size == _capacity) expand();
        _data[_size++] = x;
        _isSorted = false;
    }

    void pop_front()
    {
        if (_size == 0) return;
        if(_size >= 2)
            _data[0] = _data[_size - 1];
        _size--;
    }

    // pop from back (the last element)
    // ONLY reduce array size , NO NEED to clear data
    void pop_back()
    {
        if (_size == 0) return;
        _size--;
    }

    // erase by iterator
    // move elements forward from index
    bool erase(iterator pos) 
    {
        if (_size == 0) return false;
        *pos = *iterator(&_data[_size - 1]);
        _size--;
        _isSorted = false;
        return true;
    }

    // erase by data
    bool erase(const T &x)
    {
        if (_size == 0) return false;
        iterator it = find(x);
        if(it == end()) return false;
        return erase(it);
    }

    // reset _size to 0
    // DO NOT release memory (_capacity remain the same)
    void clear()
    {
        _size = 0;
    }

    // return a iterator of found element
    // else return a end() iterator
    iterator find(const T &x) const
    {
        if(_isSorted)
        {
            iterator it = lower_bound(begin(), end(), x);
            cout << *it << endl;
            return it;
        }
        else
        {
            for (size_t i = 0 ; i < _size ; i++)
            {
                if(_data[i] == x)
                    return iterator(&_data[i]);
            }
        }

        return end();
    }
 
    // [Optional TODO] Feel free to change, but DO NOT change ::sort()
    void sort() const
    {
        if(_isSorted)
            return;
        if (!empty())
        {
            ::sort(_data, _data + _size);
            _isSorted = true;
        }
    }
    // Nice to have, but not required in this homework...
    void reserve(size_t &n) 
    {
        if(n <= _capacity)
            return;
        T *t = new T[n];
        for (size_t i = 0; i < n; i++)
        {
            t[i] = _data[i];
        }
        if (_data != NULL)
            delete[] _data;
        _data = t;
    }

    // Drop elements that lager than n
    // and push 0 if n > original size
    void resize(size_t &n)
    {
        //if(n < _size)
    }

    size_t capacity() const { return _capacity; }

  private:
    // [NOTE] DO NOT ADD or REMOVE any data member
    T *_data;
    size_t _size;           // number of valid elements
    size_t _capacity;       // max number of elements
    mutable bool _isSorted; // (optionally) to indicate the array is sorted

    // [OPTIONAL TODO] Helper functions; called by public member functions

    // expand array when capacity <= array size
    void expand()
    {
        size_t new_capacity = 1;

        // if array is empty, new an array
        if(_size == 0)
        {
            _data = new T[1];
            _capacity = 1;
            return;
        }

        // get new size by expand size to 2^n (4,8,16...)
        while(_capacity != 0)
        {
            _capacity = _capacity >> 1;
            new_capacity = new_capacity << 1;
        }

        _capacity = new_capacity;

        // moving data to new array
        T *t = new T[new_capacity];
        for (size_t i = 0; i < _size; i++)
        {
            t[i] = _data[i];
        }
        if (_data != NULL)
            delete[] _data;
        _data = t;
    }
};

#endif // ARRAY_H
