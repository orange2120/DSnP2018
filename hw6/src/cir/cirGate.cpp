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

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_globalRef = 0;
CirGate::CirGate(unsigned &i) : _ref(0), _id(i)
{
    _id /= 2;
}

CirGate::~CirGate()
{
}

void CirGate::reportGate() const
{
    // TODO
    cout << "==================================================" << endl;
    cout << "= " << _typeStr << "(" << _id << "), line " << _lineNo << setw(33) << right << "=" << endl;
    cout << "==================================================" << endl;
}

void CirGate::reportFanin(int level) const
{
    assert(level >= 0);
}

void CirGate::reportFanout(int level) const
{
    assert(level >= 0);
}

void CirGate::addFin(CirGate *&g)
{
    _inList.push_back(g);
    g->_outList.push_back(this);
}

void CirGate::addFin2(CirGate *&g)
{
    _inList2.push_back(g);
}

void CirGate::addFout(CirGate *&g)
{
    _outList.push_back(g);
}

void CirGate::dfsTraversal(CirGate *node, GateList &l)
{
    //cerr << "GL.REF: " << _globalRef << ", REF: " << _ref << endl;
    // Reach bottom or had been traversed
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

void CirGate::PrintFiDFS(CirGate *node) const
{
    for (CirGate *&g : _inList)
    {
        if (!g->isGlobalRef())
        {
            g->setToGlobalRef();
            g->dfsTraversal(g, l);
        }
    }
    for (CirGate *&g : _inList2)
    {
        if (!g->isGlobalRef())
        {
            g->setToGlobalRef();
            g->dfsTraversal(g, l);
        }
    }
    if (isInv)
        this->printGate();
}

void CirGate::PrintFiDFS(CirGate *node) const
{
    for (CirGate *&g : _outList)
    {
        if (!g->isGlobalRef())
        {
            g->setToGlobalRef();
            g->dfsTraversal(g, l);
        }
    }
}

/**************************************/
/* class UNDEF GATE member functions  */
/**************************************/
UNDEF_gate::UNDEF_gate(unsigned &n) : CirGate(n)
{
    _typeStr = "UNDEF";
}

UNDEF_gate::~UNDEF_gate()
{
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

    if (_in1 % 2 == 0)
    {
        _in1 /= 2;
    }
    else
    {
        _in1--;
        _in1 /= 2;
        _inv1 = true;
    }
    if (_in2 % 2 == 0)
    {
        _in2 /= 2;
    }
    else
    {
        _in2--;
        _in2 /= 2;
        _inv2 = true;
    }
}

AIG_gate::~AIG_gate()
{
}

void AIG_gate::printGate() const
{
    cout << "AIG " << _id << " ";
    if (_inv1)
        cout << "!" << _in1;
    else
        cout << _in1;
    cout << " ";
    if (_inv2)
        cout << "!" << _in2;
    else
        cout << _in2;
}

/**************************************/
/*   class PI GATE member functions   */
/**************************************/
PI_gate::PI_gate(unsigned &n) : CirGate(n)
{
    _typeStr = "PI";
}

PI_gate::~PI_gate()
{
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
    if (_in % 2 == 0)
    {
        _in /= 2;
    }
    else
    {
        _in--;
        _in /= 2;
        _inv = true;
    }
}

PO_gate::~PO_gate()
{
}

void PO_gate::printGate() const
{
    cout << "PO  ";
    cout << _id << " ";
    if (_inv)
        cout << "!" << _in;
    else
        cout << _in;
}

/**************************************/
/* class CONST GATE member functions  */
/**************************************/
CONST_gate::CONST_gate(unsigned n = 0) : CirGate(n)
{
}

CONST_gate::~CONST_gate()
{
}

void CONST_gate::printGate() const
{
    cout << "CONST0";
}