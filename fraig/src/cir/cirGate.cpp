/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

// Initialize _globalRef
unsigned CirGate::_globalRef = 0;

CirGate::CirGate(unsigned &i) : _ref(0), _id(i)
{
    _symbol = NULL;
}

CirGate::~CirGate()
{
    delete _symbol;
    _symbol = NULL;
    _fin1 = NULL;
    _fin2 = NULL;
}

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void CirGate::reportGate() const
{
   string str = "";
   cout << "================================================================================" << endl;
   str = "= " + _typeStr + "(" + to_string(_id) + ")";
   if (_symbol != NULL)
      str += ("\"" + *_symbol + "\"");
   str += (", line " + to_string(_lineNo));
   cout << setw(49) << left << str << endl;
   str = "= FECs: ";
   cout << str << endl;
   str = "= Value: ";
   cout << str << endl;
   cout << "================================================================================" << endl;
}

void CirGate::reportFanin(int level) const
{
    assert(level >= 0);
    setGlobalRef();
    PrintFiDFS(this, level, 0, 0);
}

void CirGate::reportFanout(int level) const
{
    assert(level >= 0);
    setGlobalRef();
    PrintFoDFS(this, level, 0, 0);
}

// add gate to fanin 1
void CirGate::addFin1(CirGate *&g)
{
    _fin1 = g;
    g->_outList.push_back(this); // connect g's _outList to current gate
}

// add gate to fanin 2
void CirGate::addFin2(CirGate *&g)
{
    _fin2 = g;
    g->_outList.push_back(this);
}

// add a gate to fanout gate list
void CirGate::addFout(CirGate *&g)
{
   _outList.push_back(g);
}

void CirGate::dfsTraversal(CirGate *node, GateList &l)
{
    if (_fin1 != NULL)
    {
        if (!_fin1->isGlobalRef())
        {
            _fin1->setToGlobalRef();
            _fin1->dfsTraversal(_fin1, l);
        }
    }
    if (_fin2 != NULL)
    {
        if (!_fin2->isGlobalRef())
        {
            _fin2->setToGlobalRef();
            _fin2->dfsTraversal(_fin2, l);
        }
    }
    l.push_back(node);
}

void CirGate::PrintFiDFS(const CirGate *node, int &level, int depth, bool inv) const
{
    assert(level >= 0);
    if (depth > level)
        return;

    for (int i = 0; i < depth; i++)
        cout << "  ";
    if (inv)
        cout << '!';
    cout << _typeStr << ' ' << _id;

    if (depth == level)
    {
        cout << endl;
        return;
    }

    if (isGlobalRef())
        cout << " (*)" << endl;
    else
    {
        cout << endl;

        if(_fin1 != NULL)
            _fin1->PrintFiDFS(_fin1, level, depth + 1, node->_inv1);
    
        if(_fin2 != NULL)
            _fin2->PrintFiDFS(_fin2, level, depth + 1, node->_inv2);

        if (_fin1 != NULL || _fin2 != NULL)
            setToGlobalRef();
    }
}

void CirGate::PrintFoDFS(const CirGate *node, int &level, int depth, bool inv) const
{
    assert(level >= 0);
    bool finv = false;
    if (depth > level)
        return;
    for (int i = 0; i < depth; i++)
        cout << "  ";
    if (inv)
        cout << '!';
    cout << _typeStr << " " << _id << endl;
    for (CirGate *g : _outList)
    {
        if (g->_inv1 || g->_inv2)
        {
            finv = true;
        }
        if (!g->isGlobalRef())
        {
            g->PrintFoDFS(g, level, depth + 1, finv);
            if (!(g->_outList.empty()))
                g->setToGlobalRef();
        }
        else
        {
            if (depth == level)
                return;
            for (int i = 0; i < depth + 1; i++)
                cout << "  ";
            if (inv)
                cout << "!";
            cout << g->_typeStr << " " << g->_id;
            if (depth < level - 1)
                cout << " (*)";
            cout << endl;
            return;
        }
    }
}

void CirGate::OptDFS(CirGate *g, GateList &l, IdList &del)
{
    if (!g->_outList.empty())
    {
        for(CirGate *next : g->_outList)
        {
            if (!next->isGlobalRef())
            {
                next->setToGlobalRef();
                bool gateTodel = false;
        
                // PI have no effective fanin
                if (next->_typeID == PI_GATE)
                    return;

                // Fanin has constant 0 or 1
                if (next->_fin1->_typeID == CONST_GATE)
                {
                    // Fanin 1 has constant 1, replaced by the other fanin
                    if(next->_inv1)
                    {
                        next->mergeToGate(true);
                        cout << " (Fanin has constant 1)" << endl;
                    }
                    // Fanin 1 has constant 0, replaced by 0
                    else
                    {
                        next->mergeToGate(false);
                        cout << " (Fanin has constant 0)" << endl;
                    }
                    gateTodel = true;
                }
                else if (next->_fin2->_typeID == CONST_GATE)
                {
                    // Fanin 2 has constant 1
                    if(next->_inv2)
                    {
                        next->mergeToGate(false);
                        cout << " (Fanin has constant 1)" << endl;
                    }
                    else
                    {
                        next->mergeToGate(true);
                        cout << " (Fanin has constant 0)" << endl;
                    }
                    gateTodel = true;
                }
                // Identical fanins
                else if (next->_fin1 == next->_fin2)
                {
                    // Inverted fanins
                    if(next->_inv1 != next->_inv2)
                    {
                        g->mergeToGate(false);
                        cout << " (Inverted fanins)" << endl;
                    }
                    if(next->_inv1)
                        next->mergeIdentical(true);
                    else
                        next->mergeIdentical(false);
                    cout << " (Identical fanins)" << endl;

                    gateTodel = true;
                }

                if(gateTodel)
                {
                    del.push_back(next->_id);
                    l[next->_id] = NULL;
                    delete g;
                }
                next->OptDFS(next, l, del);
            }
        }
    }
    /*
    // PI have no effective fanin
                if (g->_typeID == PI_GATE)
                    return;

                // Fanin has constant 0 or 1
                if (g->_fin1->_typeID == CONST_GATE)
                {
                    // Fanin 1 has constant 1, replaced by the other fanin
                    if(g->_inv1)
                    {
                        g->mergeToGate(true);
                        cout << " (Fanin has constant 1)" << endl;
                    }
                    // Fanin 1 has constant 0, replaced by 0
                    else
                    {
                        g->mergeToGate(false);
                        cout << " (Fanin has constant 0)" << endl;
                    }
                    gateTodel = true;
                }
                else if (g->_fin2->_typeID == CONST_GATE)
                {
                    // Fanin 2 has constant 1
                    if(g->_inv2)
                    {
                        g->mergeToGate(false);
                        cout << " (Fanin has constant 1)" << endl;
                    }
                    else
                    {
                        g->mergeToGate(true);
                        cout << " (Fanin has constant 0)" << endl;
                    }
                    gateTodel = true;
                }
                // Identical fanins
                else if (g->_fin1 == g->_fin2)
                {
                    // Inverted fanins
                    if(g->_inv1 != g->_inv2)
                    {
                        g->mergeToGate(false);
                        cout << " (Inverted fanins)" << endl;
                    }
                    if(g->_inv1)
                        g->mergeIdentical(true);
                    else
                        g->mergeIdentical(false);
                    cout << " (Identical fanins)" << endl;

                    gateTodel = true;
                }

                if(gateTodel)
                {
                    del.push_back(g->_id);
                    l[g->_id] = NULL;
                    delete g;
                }
                */
}

// Remove a specify gate from fanins
void CirGate::removeFiConn(unsigned &id)
{
    if (_fin1 != NULL)
        if (_fin1->_id == id)
            _fin1 = NULL;
    if (_fin2 != NULL)
        if (_fin2->_id == id)
            _fin2 = NULL;
}

// Let Fanin gates forget myself
void CirGate::removeFiConn()
{
    if (_fin1 != NULL)
        for (unsigned i = 0; i < _fin1->_outList.size(); ++i)
        {
            if (_fin1->_outList[i] == this)
                _fin1->_outList.erase(_fin1->_outList.begin() + i);
        }
    if (_fin2 != NULL)
        for (unsigned i = 0; i < _fin2->_outList.size(); ++i)
        {
            if (_fin2->_outList[i] == this)
                _fin2->_outList.erase(_fin2->_outList.begin() + i);
        }
}

// Remove a specify gate from a fanout gate list
void CirGate::removeFoConn(unsigned &id)
{
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        if (_outList[i]->_id == id)
            _outList.erase(_outList.begin() + i);
    }
}

// Let fanout gates forget .self
void CirGate::removeFoConn()
{
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        CirGate *g = _outList[i];
        if (g->_fin1 == this)
            g->_fin1 = NULL;
        if (g->_fin2 == this)
            g->_fin2 = NULL;
    }
}

// Merge identical gates together (_fin1 == _fin2)
void CirGate::mergeIdentical(bool phase_inv)
{
    // erase self from fanin gate's fanout list
    for (unsigned i = 0; i < _fin1->_outList.size(); ++i)
    {
        if(_fin1->_outList[i] == this)
        {
            _fin1->_outList.erase(_fin1->_outList.begin() + i);
            break;
        }
    }
    // for each gate in fanout list
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        _outList[i]->_inv1 = false;
        _outList[i]->_inv2 = false;
        // connect next to prev
        // self are connected to fin1
        if (_outList[i]->_fin1 == this)
        {
            _outList[i]->_fin1 = this->_fin1;
            if(phase_inv)
                _outList[i]->_inv1 = true;
        }
        // self are connected to fin2
        else
        {
            _outList[i]->_fin2 = this->_fin1;
            if(phase_inv)
                _outList[i]->_inv2 = true;
        }
        // connect prev to next
        _fin1->_outList.push_back(_outList[i]);
    }
}

// Merge gates to a gate, the input gate is used to replace current gate.
void CirGate::mergeToGate(bool in)
{
    // TODO
    // 如果有多重output 逐一接上
    // gate to merge to next gate. False is fanin 1(CONST at fanin 1), true is fanin 2
    CirGate *g = (in) ? _fin2 : _fin1;
    // delete self from fanout's fanin
    for (unsigned i = 0; i < _fin1->_outList.size(); ++i)
    {
        if(_fin1->_outList[i] == this)
        {
            _fin1->_outList.erase(_fin1->_outList.begin() + i);
            break;
        }
    }
    for (unsigned i = 0; i < _fin2->_outList.size(); ++i)
    {
        if(_fin2->_outList[i] == this)
        {
            _fin2->_outList.erase(_fin2->_outList.begin() + i);
            break;
        }
    }

    cout << "Simplifying: " << g->_id << " merging ";
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        // connect next to prev
        if (_outList[i]->_fin1 == this)
        {
            if(_inv1)
                cout << '!';
            cout << _id;
            // connect fanouts to g
            _outList[i]->_fin1 = g;
        }
        else
        {
            if(_inv2)
                cout << '!';
            cout << _id;

            _outList[i]->_fin2 = g;
        }
        // connect prev to next
        g->_outList.push_back(_outList[i]);
    }
    cout << "...";
}

/**************************************/
/* class UNDEF GATE member functions  */
/**************************************/
UNDEF_gate::UNDEF_gate(unsigned &n) : CirGate(n)
{
    _typeStr = "UNDEF";
    _typeID = UNDEF_GATE;
}

void UNDEF_gate::printGate() const
{
    cout << "UNDEF " << _id;
}

/**************************************/
/*   class AIG GATE member functions  */
/**************************************/

AIG_gate::AIG_gate(unsigned n, unsigned &i1, unsigned &i2) : CirGate(n), _in1(i1), _in2(i2)
{
    _typeStr = "AIG";
    _typeID = AIG_GATE;

    if (_in1 % 2 != 0)
    {
        _in1--;
        _inv1 = true;
    }
    _in1 = _in1 >> 1;

    if (_in2 % 2 != 0)
    {
        _in2--;
        _inv2 = true;
    }
    _in2 = _in2 >> 1;
}

void AIG_gate::printGate() const
{
    cout << "AIG " << _id << ' ';
    if (_inv1)
        cout << '!';
    cout << _in1 << ' ';
    if (_inv2)
        cout << '!';
    cout << _in2;
    cout << endl;
}

/**************************************/
/*   class PI GATE member functions   */
/**************************************/
PI_gate::PI_gate(unsigned n) : CirGate(n)
{
   _typeStr = "PI";
   _typeID = PI_GATE;
}

void PI_gate::printGate() const
{
    cout << "PI  " << _id;
    if (_symbol != NULL) // exists comments
        cout << " (" << _symbol << ")";
    cout << endl;
}

/**************************************/
/*   class PO GATE member functions   */
/**************************************/
PO_gate::PO_gate(unsigned n, unsigned &i) : CirGate(n), _in(i)
{
   _typeStr = "PO";
   _typeID = PO_GATE;
   if (_in % 2 != 0)
   {
      _in--;
      _inv1 = true;
   }
   _in = _in >> 1;
}

void PO_gate::printGate() const
{
    cout << "PO  " << _id << " ";
    if (_inv1)
        cout << '!';
    cout << _in;
    if (_symbol != NULL) // exists comments
        cout << " (" << _symbol << ")";
    cout << endl;
}

/**************************************/
/* class CONST GATE member functions  */
/**************************************/
CONST_gate::CONST_gate(unsigned n = 0) : CirGate(n)
{
   _typeStr = "CONST";
   _typeID = CONST_GATE;
}

void CONST_gate::printGate() const
{
   cout << "CONST0" << endl;
}
