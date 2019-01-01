/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
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
    _id = _id >> 1;
    _symbol = NULL;
}

CirGate::~CirGate()
{
    delete _symbol;
    _symbol = NULL;
    _inList1.clear();
    _inList2.clear();
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

void CirGate::addFin1(CirGate *&g)
{
   _inList1.push_back(g);
   g->_outList.push_back(this); // connect g's _outList to current gate
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
   for (CirGate *&i : _inList1)
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
      for (CirGate *g : _inList1)
      {
         g->PrintFiDFS(g, level, depth + 1, node->_inv1);
      }
      for (CirGate *g : _inList2)
      {
         g->PrintFiDFS(g, level, depth + 1, node->_inv2);
      }
      if (!_inList1.empty() || !_inList2.empty())
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
PI_gate::PI_gate(unsigned &n) : CirGate(n)
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
PO_gate::PO_gate(unsigned &n, unsigned &i) : CirGate(n), _in(i)
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
