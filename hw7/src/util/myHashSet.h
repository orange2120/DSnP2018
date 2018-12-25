/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
  public:
    HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
    ~HashSet() { reset(); }

    // TODO: implement the HashSet<Data>::iterator
    // o An iterator should be able to go through all the valid Data
    //   in the Hash
    // o Functions to be implemented:
    //   - constructor(s), destructor
    //   - operator '*': return the HashNode
    //   - ++/--iterator, iterator++/--
    //   - operators '=', '==', !="
    //
    class iterator
    {
        friend class HashSet<Data>;

      public:
        iterator(const iterator &i) : _bkt(i._bkt), _bkIdx(i._bkIdx),_index(i._index), _node(i._node) {}
        iterator(const size_t idx, const size_t bkIdx, vector<Data> *b) : _bkt(b), _bkIdx(bkIdx), _index(idx), _node(&b[bkIdx][idx]) {}
        ~iterator() {}

        const Data& operator * () const { return *(this->_node); }
        Data& operator * () { return *(this->_node); }
        iterator& operator ++ ()
        {
            // bucket has more than one element
            if(_bkt[_bkIdx].size() > 1 && _index < _bkt[_bkIdx].size())
            {
                _index++;
                _node = &_bkt[_bkIdx][_index];
            }
            else
            {
                size_t tmpBktIdx = _bkIdx;
                //cerr << _bkIdx << endl;
                while (_bkt[++tmpBktIdx].empty()) { }
                if(!_bkt[tmpBktIdx].empty())
                {
                    _bkIdx = tmpBktIdx;
                }
                _index = 0;
                _node = &_bkt[_bkIdx][_index];
            }
            return *(this);
        }
        iterator operator++(int) { iterator t = *(this); ++*(this); return t; }
        iterator& operator -- ()
        {
            if(_bkt[_bkIdx].size() > 1 && _index >= 0)
            {
                _index--;
                _node = &_bkt[_bkIdx][_index];
            }
            else
            {
                while (_bkt[++_bkIdx].empty()){}
                _index = _bkt[_bkIdx].size() - 1;
                _node = &_bkt[_bkIdx][_index];
            }
            return *(this);
        }
        iterator operator -- (int) { iterator t = *(this); --*(this); return t; }
        iterator & operator = (const iterator& i) { this->_node = i._node; return (*this); }
        bool operator != (const iterator& i) const { return (i._node == _node) ? false : true; }
        bool operator == (const iterator& i) const { return (i._node == _node); }
        
      private:
        vector<Data> *_bkt;
        size_t _bkIdx;
        size_t _index;
        Data *_node;
    };

    // _buckets is an array to store vectors (of type "Data")
    //  ____________
    // | _bucket[0] |-> Data[0] -> Data[1] ......
    // | _bucket[1] |
    // | _bucket[2] |
    // | .....      |
    // |____________|

    void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
    void reset() {
        _numBuckets = 0;
        if (_buckets) { delete [] _buckets; _buckets = 0; }
    }
    void clear() {
        for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
    }
    size_t numBuckets() const { return _numBuckets; }

    vector<Data>& operator [] (size_t i) { return _buckets[i]; }
    const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

    // TODO: implement these functions
    //
    // Point to the first valid data
    iterator begin() const
    {
        for (size_t i = 0; i < _numBuckets; i++)
        {
            // find the first non-empty bucket
            if(!_buckets[i].empty())
                return iterator(0 , i, _buckets);
        }
        return iterator(0, 0, _buckets); 
    }
    // Pass the end
    iterator end() const 
    {
        for (size_t i = _numBuckets - 1; i >= 0; i--)
        {
            // find the last non-empty bucket
            if(!_buckets[i].empty())
                return iterator(_buckets[i].size() - 1 , i, _buckets);
        }
        return iterator(0, 0, _buckets);
    }
    // return true if no valid data
    bool empty() const { return (size() > 0) ? true : false; }
    // number of valid data
    size_t size() const
    { 
        size_t s = 0;
        for (size_t i = 0; i < _buckets.size(); i++)
        {
            if (_buckets[i] != NULL)
                s += _buckets[i].size();
        }
        return s; 
    }

    // check if d is in the hash...
    // if yes, return true;
    // else return false;
    bool check(const Data& d) const
    {
        size_t key = bucketNum(d);
        if (_buckets[key].empty()) // the bucket is empty
            return false;
        else
        {
            for (uint32_t i = 0; i < _buckets[key].size();i++)
            {
                if(_buckets[key][i] == d)
                    return true;
            }
        }
        return false;
    }

    // query if d is in the hash...
    // if yes, replace d with the data in the hash and return true;
    // else return false;
    bool query(Data& d) const 
    {
        size_t key = bucketNum(d);
        if (_buckets[key].empty()) // the bucket is empty
            return false;
        else
        {
            for (uint32_t i = 0; i < _buckets[key].size(); i++)
            {
                if(_buckets[key][i] == d)
                {
                    _buckets[key][i] = d;
                    return true;
                }
            }
        }
        return false;
    }

    // update the entry in hash that is equal to d (i.e. == return true)
    // if found, update that entry with d and return true;
    // else insert d into hash as a new entry and return false;
    bool update(const Data& d)
    {
        size_t key = bucketNum(d);
        if (_buckets[key].empty()) // the bucket is empty
        {
            _buckets[key].push_back(d);
            return false;
        }
        else
        {
            for (uint32_t i = 0; i < _buckets[key].size();i++)
            {
                if(_buckets[key][i] < d)
                {
                    _buckets[key][i] = d;
                    return true;
                }
            }
        }
        return false;
    }

    // return true if inserted successfully (i.e. d is not in the hash)
    // return false is d is already in the hash ==> will not insert
    bool insert(const Data& d)
    { 
        if(check(d))
            return false;
        _buckets[bucketNum(d)].push_back(d);
        return true;
    }

    // return true if removed successfully (i.e. d is in the hash)
    // return fasle otherwise (i.e. nothing is removed)
    bool remove(const Data& d)
    {
        size_t key = bucketNum(d);
        if (_buckets[key].empty()) // the bucket is empty
            return false;
        else
        {
            for (uint32_t i = 0; i < _buckets[key].size();i++)
            {
                if(_buckets[key][i] == d)
                {
                    _buckets[key].erase(_buckets[key].begin() + i);
                    return true;
                }
            }
        }
        return false;
    }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   // hash function has been finished... QQ
    size_t bucketNum(const Data& d) const {
        return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
