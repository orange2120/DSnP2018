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
    BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r) : _data(d), _left(0), _right(0) {}
    BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r, BSTreeNode<T> *p) : _data(d), _left(0), _right(0) , _parent(p) {}
    ~BSTreeNode() {}

    // [NOTE] YOU CAN ADD or REMOVE any data member
    T _data;
    BSTreeNode<T> *_left = NULL;
    BSTreeNode<T> *_right = NULL;
    BSTreeNode<T> *_parent = NULL;
};

template <class T>
class BSTree
{
    public:
    // TODO: design your own class!!

    BSTree()
    {
        // create a dummy node at first and let it be root
        _dummy = new BSTreeNode<T>(T(), NULL, NULL);
        _root = _dummy;
        //_root = NULL;
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
        iterator(BSTreeNode<T> *n = 0, BSTreeNode<T> *d = 0) : _node(n), _it_dummy(d) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++()
        {
            if(_node == _it_dummy)
            {
                _node = _it_dummy->_right; // return root
                while (_node->_left != _it_dummy)
                    _node = _node->_left;
            }
            // case 2 : if node has right child => traverse to leftmost
            else if (_node->_right != _it_dummy)
            {
                _node = _node->_right;
                while (_node->_left != _it_dummy)
                {
                    _node = _node->_left;
                }
            }
            // case 3 : parent has right child => traverse to leftmost
            else
            {
                BSTreeNode<T> *p = _node->_parent;
                while (p != _it_dummy && _node == p->_right)
                {
                    _node = p;
                    p = p->_parent;
                }
                _node = p;
            }

            return *(this);
        }
        iterator operator++(int) { iterator t = *(this); ++*(this); return t; }
        iterator &operator--()
        {
            if(_node == _it_dummy)
            {
                _node = _it_dummy->_right; // return root
                while (_node->_right != _it_dummy)
                    _node = _node->_right;
            }
            // case 2 : if node has left child => traverse to rightmost
            else if (_node->_left != _it_dummy)
            {
                _node = _node->_left;
                while (_node->_right != _it_dummy)
                {
                    _node = _node->_right;
                }
            }
            // case 3 : parent has left child => traverse to rightmost
            else
            {
                BSTreeNode<T> *p = _node->_parent;
                while (p != _it_dummy && _node == p->_left)
                {
                    _node = p;
                    p = p->_parent;
                }
                _node = p;
            }
            return *(this);
        }
        iterator operator--(int) { iterator t = *(this); --*(this); return t; }
        iterator &operator=(const iterator &i) { _node = i._node; return *(this); }

        bool operator!=(const iterator &i) const { return (_node != i._node); }
        bool operator==(const iterator &i) const { return (_node == i._node); }

        private:
        BSTreeNode<T> *_node;
        BSTreeNode<T> *_it_dummy;
    };

    iterator begin() const 
    { 
        if(_size == 0) return iterator(_dummy, _dummy);
        BSTreeNode<T> *t = GetMin(_root);
        return iterator(t, _dummy);
    }
    
    // let dummy node be end()
    iterator end() const
    { 
        return iterator(_dummy, _dummy);
    }

    bool empty() const
    {
        if (_size == 0)
            return true;
        return false;
    }

    size_t size() const { return _size; }

    void push_back(const T &x)
    {
        _root = insert_node(x, GetMin(_root));
        _size++;
    }

    void pop_front()
    {
        if (empty()) return;
        erase(iterator(GetMin(_root), _dummy));
    }

    void pop_back()
    {
        if (empty()) return;
        erase(iterator(GetMax(_root), _dummy));
    }

    void insert(const T &x)
    {
        _root = insert_node(x, _root);
    }

    // insert new BST node
    BSTreeNode<T> *insert_node(const T x, BSTreeNode<T> *root)
    {
        if(_size == 0)
        {
            // first node => root
            //BSTreeNode<T> *t = new BSTreeNode<T>(x, NULL, NULL);
            BSTreeNode<T> *t = new BSTreeNode<T>(x, _dummy, _dummy, _dummy);
            // let dummy node's children be root
            _dummy->_left = t;
            _dummy->_right = t;
            root = t;
        }
        else
        {
            if(x < root->_data)
            {
                return root->_left = insert_node(x, root->_left);
            }
            else if (x > root->_data)
            {
                return root->_right = insert_node(x, root->_right);
            }
        }
        _size++;
        return root;
    }

    // erase by iterator
    bool erase(iterator pos)
    {
        if (empty()) return false;
        remove(*pos, _root);
        _size--;
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
            _size = 0;
            return true;
        }
        _root = remove(x, _root);

        return true;
    }

    BSTreeNode<T> *remove(T x, BSTreeNode<T> *&root)
    {
        BSTreeNode<T> *tmp;
        if (root == _dummy)
            return _dummy;
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
        // TODO
        if (empty()) return false;
        _root = remove(_root->_data, _root);
        _root = _dummy;
        _size = 0;
        return true;
    }

    iterator find(const T &x) const
    {
        BSTreeNode<T> *t = search(x, _root);
        return iterator(t, _dummy);
    }

    BSTreeNode<T> *search(T x ,BSTreeNode<T> *root) const
    {
        if(root == _dummy)
            return _dummy;
        if (root->_data == x)
            return root;
        if(x < root->_data)
            return search(x, root->_left);
        else if (x > root->_data)
            return search(x, root->_right);
        return _dummy;
    }

    void inorder()
    {

    }

    void inorderPrint(BSTreeNode<T> *root) const
    {
        if(root != _dummy)
        {
            inorderPrint(root->_left);
            cout << root->_data << endl;
            inorderPrint(root->_right);
        }
    }

    void preorderPrint(BSTreeNode<T> *root) const
    {
       // _traverse_level++;
        if (root != _dummy)
        {
            //for (size_t i = 0; i < _traverse_level; i++)
            cout << "  ";
            cout << root->_data << endl; // L
            preorderPrint(root->_left);  // R
            preorderPrint(root->_right); // V
        }
        else
        {
            cout << "[0]" << endl;
        }
    }

    void sort() const
    {
    }
    
    void print() const
    {
        //inorderPrint(_root);
        preorderPrint(_root);
    }

  private:
    friend class iterator;

    size_t _size = 0;
    size_t _traverse_level = 0;
    BSTreeNode<T> *_root = 0;
    BSTreeNode<T> *_dummy = 0;


    BSTreeNode<T> *IndSuccessor(BSTreeNode<T> *n)
    {
        if(n->_right != _dummy)
            return GetMin(n->_right);
        BSTreeNode<T> *succ = n->_parent;
        while(succ != n->_parent && n == succ->_right)
        {
            n = succ;
            succ = succ->_parent;
        }
        return succ;
    }

    BSTreeNode<T> *IndPredecessor(BSTreeNode<T> *n)
    {
        if(n->_left != _dummy)
            return GetMax(n->_parent);

        BSTreeNode<T> *pred = n->_parent;
        while(pred != _dummy && n == pred->_left)
        {
            n = pred;
            pred = pred->_parent;
        }
        return pred;
    }

    BSTreeNode<T> *GetMin(BSTreeNode<T> *t) const
    {
        while(t->_left != _dummy)
        {
            t = t->_left;
        }
        return t;
    }
    BSTreeNode<T> *GetMax(BSTreeNode<T> *t) const
    {
        while(t->_right != _dummy)
        {
            t = t->_right;
        }
        return t;
    }
};

#endif // BST_H
