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
CirGate::CirGate(unsigned &i) : _id(i)
{
   _id /= 2;
}

CirGate::~CirGate()
{
}

void CirGate::reportGate() const
{
   cout << "Gate" << endl;
}

void CirGate::reportFanin(int level) const
{
   assert(level >= 0);
}

void CirGate::reportFanout(int level) const
{
   assert(level >= 0);
}

/**************************************/
/* class UNDEF GATE member functions  */
/**************************************/
void UNDEF_gate::printGate() const
{
}

/**************************************/
/*   class AIG GATE member functions  */
/**************************************/
AIG_gate::AIG_gate(unsigned &n, unsigned &i1, unsigned &i2) : CirGate(n), _in1(i1), _in2(i2)
{
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
   cout << "" << endl;
}

/**************************************/
/*   class PI GATE member functions   */
/**************************************/
PI_gate::PI_gate(unsigned &n) : CirGate(n)
{
}

PI_gate::~PI_gate()
{
}

void PI_gate::printGate() const
{
}

/**************************************/
/*   class PO GATE member functions   */
/**************************************/
PO_gate::PO_gate(unsigned &n, unsigned &i) : CirGate(n), _in(i)
{
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
}

/**************************************/
/* class CONST GATE member functions  */
/**************************************/
void CONST_gate::printGate() const
{
}