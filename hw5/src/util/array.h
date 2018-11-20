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

        // TODO: implement these overloaded operators
        const T &operator*() const { return this->_data; }
        T &operator*() { return this->_data; }
        iterator &operator++() { this->_node = _node + T_SIZE; return *(this); }
        iterator operator++(int) { return iterator(_node + T_SIZE); }
        iterator &operator--() { this->_node = _node - T_SIZE; return *(this); }
        iterator operator--(int) { return iterator(_node - T_SIZE); }
        iterator operator+(int i) const { return iterator(_node - i * T_SIZE); }
        iterator &operator+=(int i) { this->_node = _node + i * T_SIZE; return (*this); }
        iterator &operator=(const iterator &i) { this->_node = i._node; return (*this); }

        bool operator!=(const iterator &i) const { return this->_node != i._node; }
        bool operator==(const iterator &i) const { return this->_node == i._node; }

      private:
        T *_node;
    };

    // TODO: implement these functions
    iterator begin() const { return iterator(_data[0]); }
    iterator end() const { return iterator(_data[_size - 1]); }
    bool empty() const { return _size; }
    size_t size() const { return _size; }

    T &operator[](size_t i) { return _data[i]; }
    const T &operator[](size_t i) const { return _data[i]; }

    void push_back(const T &x)
    {
        if (_size == _capacity)
            expand();
        _data[_size++] = x;
        _size++;
    }

    void pop_front()
    {
        if (_size == 0)
            return;
        _data[0] = _data[_size - 1];
        size--;
    }

    // pop from back (the last element)
    // ONLY reduce array size , NO NEED to clear data
    void pop_back()
    {
        if (_size == 0)
            return;
        size--;
    }

    // erase by iterator
    // move elements forward from index
    bool erase(iterator pos) {
        if (_size == 0)
            return false;

        for (::iterator it = pos; it != end() ;it++)
        {
            it = it++;
        }

        _size--;
        return true;
    }

    // erase by data
    bool erase(const T &x)
    {
        if (_size == 0)
            return false;

        size_t idx = find(x);

        if(idx < 0)
            return false;

        for (size_t i = idx; i < _size;i++)
            _data[i] = _data[i + 1];
        _size--;
        return true;
    }

    // reset _size to 0
    // DO NOT release memory (_capacity remain the same)
    void clear()
    {
        _size = 0;
    }

    // return a iterator of found element
    // else return a end() iterator
    iterator find(T &x) 
    {
        for (size_t i = 0; i < _size;i++)
        {
            if(_data[i] == x)
                return iterator(_data[i]);
        }

        return end();
    }
    
    // return array index of found element
    // else return -1
    size_t find(T &x)
    {
        for (size_t i = 0; i < _size;i++)
        {
            if(_data[i] == x)
                return i;
        }

        return -1;
    }
 
    // [Optional TODO] Feel free to change, but DO NOT change ::sort()
    void sort() const
    {
        if (!empty())
        {
            ::sort(_data, _data + _size);
            _isSorted = true;
        }
    }
    // Nice to have, but not required in this homework...
    void reserve(size_t n) {}

    // Drop elements that lager than new  capacity
    void resize(size_t n)
    {
    }

    size_t capacity() { return _capacity; }

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
        // TODO
        size_t new_size = 1;
        
        // get new size by expand size to 2^n (4,8,16...)
        while(n != 0)
        {
            new_size = new_size << 1;
        }

        // moving data to new array
        T *t = new T[new_size];
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
