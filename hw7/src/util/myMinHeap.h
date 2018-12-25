/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
   public:
    MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
    ~MinHeap() {}

    void clear() { _data.clear(); }

    // For the following member functions,
    // We don't respond for the case vector "_data" is empty!
    const Data& operator [] (size_t i) const { return _data[i]; }   
    Data& operator [] (size_t i) { return _data[i]; }
    size_t size() const { return _data.size(); }


    // TODO
    const Data& min() const { return _data[0]; } // min is at _data[0]
    void insert(const Data& d)
    {
        size_t t = _data.size() + 1;
        _data.push_back(d);
        while (t > 1)
        {
            size_t p = t / 2; // get parent
            if(_data[p - 1] < _data[t - 1])
                break;
            swap(_data[t - 1], _data[p - 1]); // swap with its parents
            t = p;
        }
    }
    void delMin()
    {
        size_t p = 1;
        size_t t = 2 * p;
        size_t n = _data.size() + 1;
        while (t <= n)
        {
            if (t < n)
                if (_data[t] < _data[t - 1])
                    ++t;
            if (_data[n] < _data[t])
                break;
            swap(_data[p - 1], _data[t - 1]);
            p = t;
            t = 2 * p; // get child
        }
        //_data[p] = _data[n--];
    }

    void delData(size_t i)
    {
        // using down heap
        size_t t = i;
        size_t p = t / 2;
        size_t n = _data.size();
        _data[i] = _data[n];
        _data.resize(n);
        while (t <= n)
        {
            if (t < n)
                if (_data[t] < _data[n])
                    ++t;
            if (_data[n] < _data[n])
                break;
            swap(_data[p - 1], _data[t - 1]);
            p = t;
            t = 2 * p; // get child
        }
    }

  private:
      // DO NOT add or change data members
      vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
