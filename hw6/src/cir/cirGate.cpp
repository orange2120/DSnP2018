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

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

unsigned CirGate::_globalRef = 0;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
CirGate::CirGate(unsigned &i) : _ref(0), _id(i)
{
    _id /= 2;
    _symbol = NULL;
}

CirGate::~CirGate()
{
    delete _symbol;
    _symbol = NULL;
}

void CirGate::reportGate() const
{
    string str = "";
    cout << "==================================================" << endl;
    str = "= " + _typeStr + "(" + to_string(_id) + ")";
    if (_symbol != NULL)
        str += ("\"" + *_symbol + "\"");
    str += (", line " + to_string(_lineNo));
    cout << setw(49) << left << str << "=" << endl;
    cout << "==================================================" << endl;
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

void CirGate::addFin(CirGate *&g)
{
    _inList.push_back(g);
    g->_outList.push_back(this); // connect g's _outList to myself
}

void CirGate::addFin2(CirGate *&g)
{
    _inList2.push_back(g);
    g->_outList.push_back(this);
}

void CirGate::addFout(CirGate *&g)
{
    _outList.push_back(g);
}

void CirGate::dfsTraversal(CirGate *node, GateList &l)
{
    for (CirGate *&i : _inList)
    {
        if (!i->isGlobalRef())
        {
            i->setToGlobalRef();
            i->dfsTraversal(i, l);
        }
    }
    for (CirGate *&i : _inList2)
    {
        if (!i->isGlobalRef())
        {
            i->setToGlobalRef();
            i->dfsTraversal(i, l);
        }
    }
    l.push_back(node);
}

void CirGate::PrintFiDFS(const CirGate *node, int &level, int depth, bool inv) const
{
    if (depth > level)
        return;
    for (int i = 0; i < depth; i++)
        cout << "  ";
    if (inv)
        cout << '!';
    cout << _typeStr << " " << _id << endl;

    for (CirGate *g : _inList)
    {
        if (!g->isGlobalRef())
        {
            // g is successor
            g->setToGlobalRef();
            g->PrintFiDFS(g, level, depth + 1, node->_inv1);
        }
        else
        {
            if (depth == level) return;
            for (int i = 0; i < depth + 1; i++)
                cout << "  ";
            if (node->_inv1)
                cout << '!';
            cout << g->_typeStr << ' ' << g->_id;
            if (depth < level-1)
                cout << " (*)";
            cout << endl;
            return;
        }
    }
    for (CirGate *g : _inList2)
    {
        if (!g->isGlobalRef())
        {
            g->setToGlobalRef();
            g->PrintFiDFS(g, level, depth + 1, node->_inv2);
        }
        else
        {
            if (depth == level) return;
            for (int i = 0; i < depth + 1; i++)
                cout << "  ";
            if (node->_inv2)
                cout << "!";
            cout << g->_typeStr << " " << g->_id;
            if (depth < level-1)
                cout << " (*)";
            cout << endl;
            return;
        }
    }
}

void CirGate::PrintFoDFS(const CirGate *node, int &level, int depth, bool inv) const
{
    bool finv = false;
    if (depth > level) return;
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
            if (!(g->_outList.empty()))
                g->setToGlobalRef();
            else
                return;
            g->PrintFoDFS(g, level, depth + 1, finv);
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

AIG_gate::AIG_gate(unsigned &n, unsigned &i1, unsigned &i2) : CirGate(n), _in1(i1), _in2(i2)
{
    _typeStr = "AIG";
    _typeID = AIG_GATE;

    if (_in1 % 2 == 0)
        _in1 /= 2;
    else
    {
        _in1--;
        _in1 /= 2;
        _inv1 = true;
    }
    if (_in2 % 2 == 0)
        _in2 /= 2;
    else
    {
        _in2--;
        _in2 /= 2;
        _inv2 = true;
    }
}

void AIG_gate::printGate() const
{
    cout << "AIG " << _id << ' ';
    if (_inv1)
        cout << '!';
    cout << _in1 << ' ';
    if (_inv2)
        cout << "!";
    cout << _in2;
}

/**************************************/
/*   class PI GATE member functions   */
/**************************************/
PI_gate::PI_gate(unsigned &n) : CirGate(n)
{
    _typeStr = "PI";
    _typeID = PI_GATE;
}

void PI_gate::printGate() const
{
    cout << "PI  " << _id;
}

/**************************************/
/*   class PO GATE member functions   */
/**************************************/
PO_gate::PO_gate(unsigned &n, unsigned &i) : CirGate(n), _in(i)
{
    _typeStr = "PO";
    _typeID = PO_GATE;
    if (_in % 2 == 0)
        _in /= 2;
    else
    {
        _in--;
        _in /= 2;
        _inv1 = true;
    }
}

void PO_gate::printGate() const
{
    cout << "PO  ";
    cout << _id << " ";
    if (_inv1)
        cout << '!';
    cout << _in;
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
    cout << "CONST0";
}