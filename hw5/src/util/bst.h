/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
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

    // TODO: design your own class!!
    BSTreeNode(const T &d, const BSTreeNode<T> *l, const BSTreeNode<T> *r) : _data(d), _left(0), _right(0), _parent(p) {}

    // [NOTE] YOU CAN ADD or REMOVE any data member
    T _data;
    BSTreeNode<T> *_left = NULL;
    BSTreeNode<T> *_right = NULL;
    BSTreeNode<T> *_parent;
};

template <class T>
class BSTree
{
    public:
    // TODO: design your own class!!

    BSTree()
    {
        _dummy = new BSTreeNode<T>(T(), NULL, NULL);
        _root = _dummy;
    }

    ~BSTree() 
    {
        clear();
        delete _root;
    }


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
            iterator operator++(int) { return iterator(); }
            iterator &operator--()
            {
                return *(this);
            }
            iterator operator--(int) { return iterator(); }
            iterator &operator=(const iterator &i) { _node = i._node; return *(this); }

            bool operator!=(const iterator &i) const { return (this->_node != i._node); }
            bool operator==(const iterator &i) const { return (this->_node == i._node); }

          private:
            BSTreeNode<T> *_node;
        };

    iterator begin() const 
    { 
        if(_size == 0) return 0;
         return iterator(); 
    }
    
    iterator end() const 
    { 
        if(_size == 0) return 0;
        return iterator(_dummy);
    }

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
        _size++;
    }

    void pop_front()
    {
        if (empty())
            return;
        _size--;
    }

    void pop_back()
    {
        if (empty())
            return;
        _size--;
    }

    void insert(T &x)
    {
        insert_node(x, _root);
    }

    // insert new BST node
    void insert_node(T &x, BSTreeNode<T> *root)
    {
        if(root == NULL)
        {
            BSTreeNode<T> *t = new BSTreeNode<T>(x, NULL, NULL);
            root = t;
        }
        else
        {
            if(x < root->_data)
                return root->_left = insert(x, root->_left);
            else if (x > root->_data)
                return root->_right = insert(x, root->_right);
        }
        _size++;
        return root;
    }

    // erase by iterator
    bool erase(iterator pos)
    {
        if (empty())
            return false;

        remove(*pos, _root);

        return true;
    }

    // erase by data
    bool erase(const T &x)
    {
        if (empty()) return false;
        if(_size == 1) // only exist _root
        {
            delete _root;
            _root = _dummy;
            return true;
        }
        remove(x, _root);

        return true;
    }

    BSTreeNode<T> *remove(T &x, BSTreeNode<T> *&root)
    {
        BSTreeNode<T> *tmp;
        if (root == NULL)
            return NULL;
        if(x < root->_data)
            root->_left = remove(x, root->_left);
        else if (x > root->_data)
            root->_right = remove(x, root->_right);
        else // root->_data == x
        {
            if(root->_left && root->_right)
            {
                tmp = GetMax(root->_left);
                root->_data = tmp->_data;
                root->_left = remove(x, root->_left);
            }

            else
            {
                tmp = root;
                if(root->_left == NULL)
                    root = root->_right;
                if(root->_right == NULL)
                    root = root->_left;
                delete tmp;
            }
        }
        _size--;
        return root;
    }

    // delete all the dlist nodes but dummy node
    bool clear()
    {
        if (empty()) return false;
        return true;
    }

    iterator find(T &x) const
    {
        BSTreeNode<T> *t = find(x, _root);
        return t;
    }

    BSTreeNode<T> *search(T &x ,BSTreeNode<T> *&root) const
    {
        if(root == NULL)
            return NULL;

        if(root->_data == x)
            return root;
        if(x < root->_data)
            return search(x, root->_left);
        else if (x > root->_data)
            return search(x, root->_right);
        return NULL;
    }

    void sort() const
    {
    }
    
    void print()
    {

    }


  private:
    size_t _size = 0;
    BSTreeNode<T> *_root = 0;
    BSTreeNode<T> *_dummy = 0;
    BSTreeNode<T> *successor(BSTreeNode<T> &n) {}

    BSTreeNode<T> *GetMin(BSTreeNode<T> *&t)
    {
        if(t->_left == NULL)
            return t;
        else
            return t->_left;
    }
    BSTreeNode<T> *GetMax(BSTreeNode<T> *&t)
    {
        if(t->_right == NULL)
            return t;
        else
            return t->_right;
    }
};

#endif // BST_H
