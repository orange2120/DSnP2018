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
    
    // store duplicate node number
    u_int8_t _cnt = 0;
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

        // [IMPORTANT] pass _dummy node into constructor, to make traversal easier
        iterator(BSTreeNode<T> *n = 0, BSTreeNode<T> *d = 0) : _node(n), _it_dummy(d) {}
        iterator(const iterator &i) : _node(i._node) {}
        ~iterator() {} // Should NOT delete _node

        const T &operator*() const { return _node->_data; }
        T &operator*() { return _node->_data; }
        iterator &operator++()
        {
            /*
            if(_traversal_times > 0)
            {
                _traversal_times--;
                return *(this);
            }

            if(_node->_cnt > 1)
            {
                _isMarked = true;
                _traversal_times = _node->_cnt;
            }*/

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
                // record current node for comparison with found node
                //BSTreeNode<T> *tmp = _node;
                _node = _node->_right;
                while (_node->_left != _it_dummy)
                {
                    _node = _node->_left;
                }
                /*tmp = tmp->_right;
                while (tmp->_left != _it_dummy)
                {
                    tmp = tmp->_left;
                }
                if(tmp->_data == _node)
                {
                    
                }*/
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
        u_int8_t _traversal_times = 0;
        bool _isMarked = false;
    };

    iterator begin() const 
    { 
        if(_size == 0) return iterator(_dummy, _dummy);
        //BSTreeNode<T> *t = GetMin(_root);
        return iterator(GetMin(_root), _dummy);
    }
    
    // Let dummy node be at end()
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
        erase(iterator(GetMin(_root), _dummy));
    }

    void pop_back()
    {
        if (_size == 0) return;
        erase(iterator(GetMax(_root), _dummy));
    }

    void insert(const T &x)
    {
        _root = insert_node(x, _root);
        //insert_node(x);
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
            t->_cnt++;
            return t;
        }

        // duplicate node and current node has left child
        if (x == node->_data && node->_left != _dummy)
        {
            // move left child to duplicate node left child
            BSTreeNode<T> *left = node->_left;
            BSTreeNode<T> *t = new BSTreeNode<T>(x, left, _dummy, node);
            node->_left = t;
            left->_parent = t;
            (node->_cnt)++;
            return t;
        }
        
        else if (x == node->_data && node->_left == _dummy)
        {
            BSTreeNode<T> *t = new BSTreeNode<T>(x, _dummy, _dummy, node);
            node->_left = t;
            (node->_cnt)++;
            return t;
        }
        else if(x < node->_data)
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
    
   /*
    void insert_node(const T &d)
    {
        

        if(_size == 0) // also _root == _dummy
        {
            // first node => root
            BSTreeNode<T> *t = new BSTreeNode<T>(d, _dummy, _dummy, _dummy);
            // Let dummy node's children be root
            _dummy->_left = t;
            _dummy->_right = t;
            _root = t;
        }

        BSTreeNode<T> *y = _dummy;
        BSTreeNode<T> *x = _dummy;

        BSTreeNode<T> *node = new BSTreeNode<T>(d, _dummy, _dummy, _dummy);

        x = _root;
        while(x != _dummy)
        {
            y = x;
            if(d > (node->_data))
                x = x->_left;
            else x = x->_right;
        }
        

        node->_parent = y;
        if(y == _dummy)
            _root = node;
        else if(d > (node->_data))
            y->_left = node;
        else
            y->_right = node;
    }
    */

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

        // if there exists duplicate
        /*if(del->_cnt > 1)
        {
            (del->_cnt)--;
            return true;
        }*/
        //_root = delete_node(x, _root);
        delete_node(del);
        _size--;
        return true;
    }

    /*
    BSTreeNode<T> *delete_node(const T &x, BSTreeNode<T> *node)
    {
        BSTreeNode<T> *tmp; // temp node for deletion
        
        // root
        if (node == _dummy)
            return _dummy;
        
        // find position to delete
        if(x < node->_data)
            node->_left = delete_node(x, node->_left);
        else if (x > node->_data)
            node->_right = delete_node(x, node->_right);
        else // root->_data == x
        {
            // the node has only L or R child
            if(node->_left == _dummy)
            {
                tmp = node->_right;
                delete node;
                return tmp;
            }
            else if(node->_right == _dummy)
            {
                tmp = node->_left;
                delete node;
                return tmp;
            }

            // if the node has two children
            // get inorder minimum value 
            tmp = GetMin(node->_right);

            node->_data = tmp->_data;

            node->_right = delete_node(tmp->_data, node->_right);
        }

        return node;
    }*/

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
            tmp == IndSuccessor(node);
        
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

    // delete all the dlist nodes but dummy node
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
        if(node == _dummy)
            return _dummy;
        // found target
        if (node->_data == x)
            return node;
        
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

    void print() const
    {
        //inorderPrint(_root);
        preorderPrint(_root);
    }

    void sort() const {}

  private:
    friend class iterator;

    size_t _size = 0;
    size_t _traverse_level = 0;
    BSTreeNode<T> *_root = 0;
    BSTreeNode<T> *_dummy = 0;
    bool _duplicate = false;

    // Iteratively find the inorder successor
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

    // Iteratively find the inorder predecessor
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
