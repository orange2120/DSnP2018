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

// For tree node color
#define BLACK 0
#define RED   1

template <class T> class BSTree;

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
   BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r) : _data(d), _left(l), _right(r) {}
   BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r, bool c) : _data(d), _left(l), _right(r), _color(c) {}
   BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r, BSTreeNode<T> *p, bool c) : _data(d), _left(l), _right(r) , _parent(p), _color(c) {}
//    BSTreeNode(const T &d, BSTreeNode<T> *l, BSTreeNode<T> *r, BSTreeNode<T> *p) : _data(d), _left(l), _right(r) , _parent(p) {}
   ~BSTreeNode() {}

   T _data;
   bool _color = 0;
   BSTreeNode<T> *_left = NULL;
   BSTreeNode<T> *_right = NULL;
   BSTreeNode<T> *_parent = NULL;
};

// This is a Red-Black Tree version
template <class T>
class BSTree
{
   // TODO: design your own class!!

  public:
    
    BSTree()
    {
        // create a nil node at first then let it be root, nil is black
        _nil = new BSTreeNode<T>(T(), NULL, NULL, BLACK);
        _nil->_left = _nil->_right = _nil;
        _root = _nil;
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
        iterator(BSTreeNode<T> *n = 0, BSTreeNode<T> *ni = 0) : _node(n), _it_nil(ni) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node
        

        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++()
        {
            // case 1 : if the node is dummy node
            if(_node == _it_nil)
            {
                _node = _it_nil->_right; // return root
                while (_node->_left != _it_nil)
                    _node = _node->_left;
            }
            // case 2 : if node has right child => traverse to its leftmost
            else if (_node->_right != _it_nil)
            {
                // record current node for comparison with found node
                _node = _node->_right;
                while (_node->_left != _it_nil)
                    _node = _node->_left;
            }
            // case 3 : parent has no right child => traverse to its parent's leftmost
            else
            {
                BSTreeNode<T> *p = _node->_parent;
                while (p != _it_nil && _node == p->_right)
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
            if(_node == _it_nil)
            {
                _node = _it_nil->_right; // return root
                while (_node->_right != _it_nil)
                    _node = _node->_right;
            }
            // case 2 : if node has left child => traverse to rightmost
            else if (_node->_left != _it_nil)
            {
                _node = _node->_left;
                while (_node->_right != _it_nil)
                {
                    _node = _node->_right;
                }
            }
            // case 3 : parent has left child => traverse to rightmost
            else
            {
                BSTreeNode<T> *p = _node->_parent;
                while (p != _it_nil && _node == p->_left)
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
        BSTreeNode<T> *_it_nil;
    };

    iterator begin() const 
    { 
        if(_size == 0) return iterator(_nil, _nil);
        return iterator(getMin(_root), _nil);
    }
    
    // Let dummy node be end()
    iterator end() const
    { 
        return iterator(_nil, _nil);
    }

    bool empty() const
    {
        return (_size == 0);
    }

    size_t size() const { return _size; }

    void pop_front()
    {
        if (_size == 0) return;
        erase(iterator(getMin(_root), _nil));
    }

    void pop_back()
    {
        if (_size == 0) return;
        erase(iterator(getMax(_root), _nil));
    }

    void insert(const T &d)
    {
        if(_size == 0) // also _root == _nil
        {
            // first node => root, root is black
            BSTreeNode<T> *t = new BSTreeNode<T>(d, _nil, _nil, _nil, BLACK);
            // Let dummy node's children be root
            _nil->_left = t;
            _nil->_right = t;
        }

        BSTreeNode<T> *z = new BSTreeNode<T>(d, _nil, _nil, _nil, RED);
        BSTreeNode<T> *y = _nil;
        BSTreeNode<T> *x = _root;
        while (x != _nil)
        {
            y = x;
            if (z->_data < x->_data)
                x = x->_left;
            else
                x = x->_right;
        }
        z->_parent = y;
        if (y == _nil)
            _root = z;
        else if (z->_data < y->_data)
            y->_left = z;
        else
            y->_right = z;
        z->_left = _nil;
        z->_right = _nil;
        z->_color = RED;

        insertFixUp(z);
        _size++;
    }

    void insertFixUp(BSTreeNode<T> *z)
    {
        while (z->_parent->_color == RED)
        {
            if (z->_parent == z->_parent->_parent->_left)
            {
                BSTreeNode<T> *y = z->_parent->_parent->_right;
                if (y->_color == RED)
                {
                    z->_parent->_color = BLACK;
                    y->_color = BLACK;
                    z->_parent->_parent->_color = RED;
                    z = z->_parent->_parent;
                }
                else
                {
                    if (z == z->_parent->_right)
                    {
                        z = z->_parent;
                        leftRotate(z);
                    }
                    z->_parent->_color = BLACK;
                    z->_parent->_parent->_color = RED;
                    rightRotate(z->_parent->_parent);
                }
            }
            else
            {
                BSTreeNode<T> *y = z->_parent->_parent->_left;
                if (y->_color == RED)
                {
                    z->_parent->_color = BLACK;
                    y->_color = BLACK;
                    z->_parent->_parent->_color = RED;
                    z = z->_parent->_parent;
                }
                else
                {
                    if (z == z->_parent->_left)
                    {
                        z = z->_parent;
                        rightRotate(z);
                    }
                    z->_parent->_color = BLACK;
                    z->_parent->_parent->_color = RED;
                    leftRotate(z->_parent->_parent);
                }
            }
        }
        _root->_color = BLACK;
    }

    void leftRotate(BSTreeNode<T> *x)
    {
        BSTreeNode<T> *y = x->_right;
        x->_right = y->_left;
        if (y->_left != _nil)
            y->_left->_parent = x;
        y->_parent = x->_parent;
        if (x->_parent == _nil)
            _root = y;
        else if (x == x->_parent->_left)
            x->_parent->_left = y;
        else
            x->_parent->_right = y;
        y->_left = x;
        x->_parent = y;
    }

    void rightRotate(BSTreeNode<T> *x)
    {
        BSTreeNode<T> *y = x->_left;
        x->_left = y->_right;
        if (y->_right != _nil)
            y->_right->_parent = x;
        y->_parent = x->_parent;
        if (x->_parent == _nil)
            _root = y;
        else if (x == x->_parent->_right)
            x->_parent->_right = y;
        else
            x->_parent->_left = y;
        y->_right = x;
        x->_parent = y;
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
        if (del == _nil)
            return false;
        if(_size == 1) // only exist _root and element exists
        {
            delete _root;
            _root = _nil;
            _nil->_parent = _nil;
            _nil->_left = _nil->_right = _nil;
            _size = 0;
            return true;
        }
        delete_node(del);
        _size--;
        return true;
    }

    void delete_node(BSTreeNode<T> *z)
    {
        BSTreeNode<T> *y = z;
        BSTreeNode<T> *x = _nil; // temporary use
        bool yColor = y->_color;

        if (z->_left == _nil)
        {
            x = z->_right;
            transPlant(z, z->_right);
        }
        else if (z->_right == _nil)
        {
            x = z->_left;
            transPlant(z, z->_left);
        }
        else
        {
            y = getMin(z->_right);
            yColor = y->_color;
            x = y->_right;
            if (y->_parent == z)
                x->_parent = y;
            else
            {
                transPlant(y, y->_right);
                y->_right = z->_right;
                y->_right->_parent = y;
            }
            transPlant(z, y);
            y->_left = z->_left;
            y->_left->_parent = y;
            y->_color = z->_color;
        }
        if (yColor == BLACK)
            deleteFixUp(x);
        delete z;
    }

    void deleteFixUp(BSTreeNode<T> *x)
    {
        while (x != _root && x->_color == BLACK)
        {
            BSTreeNode<T> *w = _nil;
            if (x == x->_parent->_left)
            {
                w = x->_parent->_right;
                if (w->_color == RED)
                {
                    w->_color = BLACK;
                    x->_parent->_color = RED;
                    leftRotate(x->_parent);
                    w = x->_parent->_right;
                }
                if (w->_left->_color == BLACK && w->_right->_color == BLACK)
                {
                    w->_color = RED;
                    x = x->_parent;
                }
                else
                {
                    if (w->_right->_color == BLACK)
                    {
                        w->_left->_color = BLACK;
                        w->_color = RED;
                        rightRotate(w);
                        w = x->_parent->_right;
                    }
                    w->_color = x->_parent->_color;
                    x->_parent->_color = BLACK;
                    w->_right->_color = BLACK;
                    leftRotate(x->_parent);
                    x = _root;
                }
            }
            else
            {
                w = x->_parent->_left;
                if (w->_color == RED)
                {
                    w->_color = BLACK;
                    x->_parent->_color = RED;
                    rightRotate(x->_parent);
                    w = x->_parent->_left;
                }
                if (w->_right->_color == BLACK && w->_left->_color == BLACK)
                {
                    w->_color = RED;
                    x = x->_parent;
                }
                else
                {
                    if (w->_left->_color == BLACK)
                    {
                        w->_right->_color = BLACK;
                        w->_color = RED;
                        leftRotate(w);
                        w = x->_parent->_left;
                    }
                    w->_color = x->_parent->_color;
                    x->_parent->_color = BLACK;
                    w->_left->_color = BLACK;
                    rightRotate(x->_parent);
                    x = _root;
                }
            }
        }

    }

    void transPlant(BSTreeNode<T> *u, BSTreeNode<T> *v)
    {
        if (u->_parent == _nil)
            _root = v;
        else if (u == u->_parent->_left)
            u->_parent->_left = v;
        else
            u->_parent->_right = v;
        v->_parent = u->_parent;
    }

    // delete all the BST nodes but dummy node
    bool clear()
    {
        if (_size == 0) return false;
        suicide(_root);
        _root = _nil;
        _nil->_parent = _nil;
        _nil->_left = _nil->_right = _nil;
        _size = 0;
        return true;
    }

    BSTreeNode<T> *suicide(BSTreeNode<T> *node)
    {
        // if reach root, delete it and set _root to dummy node
        if(node != _nil)
        {
            BSTreeNode<T> *tmp = node;
            node->_left = suicide(node->_left);
            node->_right = suicide(node->_right);
            delete tmp;
            node = _nil;
            return node;
        }
        return _nil;
    }

    iterator find(const T &x) const
    {
        BSTreeNode<T> *t = search(x, _root);
        return iterator(t, _nil);
    }

    BSTreeNode<T> *search(const T &x ,BSTreeNode<T> *node) const
    {
        // empty
        if(node == _nil) return _nil;
        // found target
        if (node->_data == x) return node;
        
        // recursively searching
        if (x < node->_data)
            return search(x, node->_left);
        else if (x > node->_data)
            return search(x, node->_right);
        
        return _nil;
    }

    void inorderPrint(BSTreeNode<T> *root) const
    {
        if(root != _nil)
        {
            inorderPrint(root->_left);
            cout << root->_data << endl;
            inorderPrint(root->_right);
        }
    }

    void preorderPrint(BSTreeNode<T> *root) const
    {
        cout << "  ";
        if (root != _nil)
        {
           cout << "  " << root->_data << endl; // L
           preorderPrint(root->_left);          // R
           preorderPrint(root->_right);         // V
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
    BSTreeNode<T> *_nil = 0; // nil node

    // Iteratively find the inorder successor
    BSTreeNode<T> *IndSuccessor(BSTreeNode<T> *n)
    {
        if(n->_right != _nil)
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
        if(n->_left != _nil)
            return getMax(n->_parent);

        BSTreeNode<T> *pred = n->_parent;
        while(pred != _nil && n == pred->_left)
        {
            n = pred;
            pred = pred->_parent;
        }
        return pred;
    }
    // Iteratively find the leftmost element
    BSTreeNode<T> *getMin(BSTreeNode<T> *t) const
    {
        while(t->_left != _nil)
            t = t->_left;
        return t;
    }
    // Iteratively find the rightmost element
    BSTreeNode<T> *getMax(BSTreeNode<T> *t) const
    {
        while(t->_right != _nil)
            t = t->_right;
        return t;
    }
};

#endif // BST_H
