/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
  q            [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T>
class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
    friend class BSTree<T>;
    friend class BSTree<T>::iterator;

    BSTreeNode(const T &d) : _data(d) {}
    // TODO: design your own class!!
    T _data;
    T *_lChild;
    T *_rChild;
};

template <class T>
class BSTree
{
    // TODO: design your own class!!
    class iterator
    {
        friend class BSTree;

      public:
        iterator(BSTreeNode<T> *n = 0) : _node(n) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++()
        {
            return *(this);
        }
        iterator operator++(int) { return iterator(_node->); }
        iterator &operator--()
        {
            return *(this);
        }
        iterator operator--(int) { return iterator(); }
        iterator &operator=(const iterator &i)
        {
            return *(this);
        }

        bool operator!=(const iterator &i) const { return this->_node != i._node; }
        bool operator==(const iterator &i) const { return (this->_node == i._node); }

      private:
        BSTreeNode<T> *_node;
    };

  public:
    bool empty() const
    {
        if (_size == 0)
            return true;
        else
            return false;
    }

    size_t size() { return _size; }

    void push_back(const T &x)
    {
    }

    void pop_front()
    {
        if (empty())
            return;
    }

    void pop_back()
    {
        if (empty())
            return;
    }

    // insert new BST node
    void insert(T &x)
    {
        BSTreeNode<T> *t = new BSTreeNode<T>(x);
    }

    // erase by iterator
    bool erase(iterator pos)
    {
        if (empty())
            return false;
        return true;
    }

    // erase by data
    bool erase(const T &x)
    {
        if (empty())
            return false;
        return true;
    }

    // delete all the dlist nodes but dummy node
    bool clear()
    {
        if (empty())
            return false;
        return true;
    }

    iterator find(T &x)
    {
    }

    void sort()
    {
    }

  private:
    BSTreeNode<T> *_root;

    BSTreeNode<T>* successor(BSTreeNode<T> &n) {}
};

#endif // BST_H
