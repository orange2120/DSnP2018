/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
  public:
    CirGate() {}
    virtual ~CirGate() {}

    // Basic access methods
    string getTypeStr() const { return ""; }
    unsigned getLineNo() const { return 0; }

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;

  private:
    pi_list<>;
    po_list<>;

  protected:
};

class and_gate : CirGate
{
  public:
  private:
};

class inv_gate : CirGate
{
  public:
  private:
};

class or_gate : CirGate
{
  public:
  private:
};

class const_gate : CirGate
{
  public:
  private:
};

class undef_gate : CirGate
{
  public:
  private:
};

#endif // CIR_GATE_H
