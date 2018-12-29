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

    const Data& min() const { return _data[0]; } // min is at _data[0]
    void insert(const Data& d)
    {
        size_t t = _data.size() + 1;
        _data.push_back(d);
        while (t > 1)
        {
            size_t p = t / 2; // get parent
            //if(_data[p - 1] < _data[t - 1])
            if(!(_data[t - 1] < _data[p - 1]))
                break;
            swap(_data[t - 1], _data[p - 1]); // swap with its parents
            t = p;
        }
    }

    void delMin()
    {
        delData(0);
    }

    void delData(size_t i)
    {
        i++;
        size_t t = 2 * i;
        size_t n = _data.size();

        _data[i - 1] = _data[n - 1];
        //_data.pop_back();
    
        // heapfying 
        while (t <= n)
        {
            if (t < n)
                // the right child is larger
                if (_data[t] < _data[t - 1])
                    ++t;
            //if(_data[n - 1] < _data[t - 1])
            if(!(_data[t - 1] < _data[n - 1]))
                break;
            _data[i - 1] = _data[t - 1];
            //swap(_data[i - 1], _data[t - 1]);
            i = t;
            t = 2 * i; // get child
        }
        swap(t, i);
        /*
        while(i * 2 <= n)
        {
            if (i * 2 + 1 > n)
                t = i * 2;
            else
            {
                //           Left             RIGHT
                if (!(_data[i * 2 - 1] < _data[i * 2]))
                //if (_data[i * 2] < _data[i * 2 - 1])
                    t = 2 * i;
                else
                    t = 2 * i + 1;
            }
            if (_data[t - 1] < _data[i - 1])
            //if (!(_data[i - 1] < _data[t - 1]))
            _data[t - 1] = _data[i - 1];
            //swap(_data[t - 1], _data[i - 1]);
            i = t;
        }
        */
        while (t > 1)
        {
            i = t / 2; // get parent
            //if(_data[p - 1] < _data.back())
            if(!(_data.back() < _data[i - 1]))
                break;
            _data[t - 1] = _data[i - 1];  // swap with its parents
            //swap(_data[t - 1], _data[i - 1]); // swap with its parents
            t = i;
        }
        _data[t - 1] = _data.back();
        _data.pop_back();
    }

  private:
      // DO NOT add or change data members
      vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
