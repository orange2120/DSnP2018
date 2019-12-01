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

    // DONE: design my own class!!
    BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r) : _data(d), _left(l), _right(r) {}
    BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r, BSTreeNode<T> *p) : _data(d), _left(l), _right(r) , _parent(p) {}
    ~BSTreeNode() {}

    // [NOTE] I CAN ADD or REMOVE any data member XDD
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
        _dummy->_left = _dummy->_right = _dummy;
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

        // [IMPORTANT] pass _dummy node into constructor to make traversal easier
        iterator(BSTreeNode<T> *n = 0, BSTreeNode<T> *d = 0) : _node(n), _it_dummy(d) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++()
        {
            // case 1 : if the node is dummy node
            if(_node == _it_dummy)
            {
                _node = _it_dummy->_right; // return root
                while (_node->_left != _it_dummy)
                    _node = _node->_left;
            }
            // case 2 : if node has right child => traverse to its leftmost
            else if (_node->_right != _it_dummy)
            {
                // record current node for comparison with founded node
                _node = _node->_right;
                while (_node->_left != _it_dummy)
                    _node = _node->_left;
            }
            // case 3 : parent has no right child => traverse to its parent's leftmost
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
        return iterator(getMin(_root), _dummy);
    }
    
    // Let dummy node be end()
    iterator end() const
    { 
        return iterator(_dummy, _dummy);
    }

    bool empty() const
    {
        if (_size == 0) return true;
        return false;
    }

    size_t size() const { return _size; }

    void pop_front()
    {
        if (_size == 0) return;
        erase(iterator(getMin(_root), _dummy));
    }

    void pop_back()
    {
        if (_size == 0) return;
        erase(iterator(getMax(_root), _dummy));
    }

    void insert(const T &x)
    {
        _root = insert_node(x, _root);
        _size++;
    }

    
    // insert new BST node
    BSTreeNode<T> *insert_node(const T x, BSTreeNode<T> *node)
    {
        if(_size == 0) // also _root == _dummy
        {
            // first node => root
            BSTreeNode<T> *t = new BSTreeNode<T>(x, _dummy, _dummy, _dummy);
            // Let dummy node's children be root
            _dummy->_left = t;
            _dummy->_right = t;
            node = t;
            return t;
        }
        // new position to insert
        if(node == _dummy)
        {
            BSTreeNode<T> *t = new BSTreeNode<T>(x, _dummy, _dummy, node);
            return t;
        }
        else if(x <= node->_data)
        {
            BSTreeNode<T> *left = insert_node(x, node->_left);
            node->_left = left;

            left->_parent = node;
        }
        else if (x > node->_data)
        {
            BSTreeNode<T> *right = insert_node(x, node->_right);
            node->_right = right;

            right->_parent = node;
        }

        return node;
    }

    // erase by iterator
    bool erase(iterator pos)
    {
        if (_size == 0) return false;
        erase(*pos);
        return true;
    }

    // erase by data
    bool erase(const T &x)
    {
        if (_size == 0) return false;
        BSTreeNode<T> *del;
        del = search(x, _root);
        if (del == _dummy)
            return false;
        if(_size == 1) // only exist _root and element exists
        {
            delete _root;
            _root = _dummy;
            _dummy->_parent = _dummy;
            _dummy->_left = _dummy->_right = _dummy;
            _size = 0;
            return true;
        }
        delete_node(del);
        _size--;
        return true;
    }

    void delete_node(BSTreeNode<T> *node)
    {
        // node for deletion
        BSTreeNode<T> *tmp;
        // child to be delete
        BSTreeNode<T> *ch = _dummy;

        // case 1 : the node has one child
        if(node->_left == _dummy || node->_right == _dummy)
            tmp = node;
        else
            tmp = IndSuccessor(node);
        
        if(tmp->_left != _dummy)
            ch = tmp->_left;
        else
            ch = tmp->_right;

        if(ch != _dummy)
            ch->_parent = tmp->_parent;
        
        if(tmp->_parent == _dummy)
            this->_root = ch;
        
        else if(tmp == tmp->_parent->_left)
            tmp->_parent->_left = ch;
        
        else
            tmp->_parent->_right = ch;

        if(tmp != node)
            node->_data = tmp->_data;
        delete tmp;
    }

    // delete all the BST nodes but dummy node
    bool clear()
    {
        if (_size == 0) return false;
        suicide(_root);
        _root = _dummy;
        _dummy->_parent = _dummy;
        _dummy->_left = _dummy->_right = _dummy;
        _size = 0;
        return true;
    }

    BSTreeNode<T> *suicide(BSTreeNode<T> *node)
    {
        // if reach root, delete it and set _root to dummy node
        if(node != _dummy)
        {
            BSTreeNode<T> *tmp = node;
            node->_left = suicide(node->_left);
            node->_right = suicide(node->_right);
            delete tmp;
            node = _dummy;
            return node;
        }
        return _dummy;
    }

    iterator find(const T &x) const
    {
        BSTreeNode<T> *t = search(x, _root);
        return iterator(t, _dummy);
    }

    BSTreeNode<T> *search(const T &x ,BSTreeNode<T> *node) const
    {
        // empty
        if(node == _dummy) return _dummy;
        // found target
        if (node->_data == x) return node;
        
        // recursively searching
        if (x < node->_data)
            return search(x, node->_left);
        else if (x > node->_data)
            return search(x, node->_right);
        
        return _dummy;
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
        cout << "  ";
        if (root != _dummy)
        {
           cout << "  " << root->_data << endl; // L
           preorderPrint(root->_left);  // R
           preorderPrint(root->_right); // V
        }
        else
        {
            cout << "[0]" << endl;
        }
    }

    void print() const
    {
        //inorderPrint(_root);
        preorderPrint(_root);
    }

    void sort() const {} // no need to implement

  private:
    size_t _size = 0;
    BSTreeNode<T> *_root = 0;
    BSTreeNode<T> *_dummy = 0; // dummy node

    // Iteratively find the inorder successor
    BSTreeNode<T> *IndSuccessor(BSTreeNode<T> *n)
    {
        if(n->_right != _dummy)
            return getMin(n->_right);
        BSTreeNode<T> *succ = n->_parent;
        while(succ != n->_parent && n == succ->_right)
        {
            n = succ;
            succ = succ->_parent;
        }
        return succ;
    }

    // Iteratively find the inorder predecessor
    BSTreeNode<T> *IndPredecessor(BSTreeNode<T> *n)
    {
        if(n->_left != _dummy)
            return getMax(n->_parent);

        BSTreeNode<T> *pred = n->_parent;
        while(pred != _dummy && n == pred->_left)
        {
            n = pred;
            pred = pred->_parent;
        }
        return pred;
    }
    // Iteratively find the leftmost element
    BSTreeNode<T> *getMin(BSTreeNode<T> *t) const
    {
        while(t->_left != _dummy)
            t = t->_left;
        return t;
    }
    // Iteratively find the rightmost element
    BSTreeNode<T> *getMax(BSTreeNode<T> *t) const
    {
        while(t->_right != _dummy)
            t = t->_right;
        return t;
    }
};

#endif // BST_H
