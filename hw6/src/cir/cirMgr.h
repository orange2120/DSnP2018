/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirGate.h"
#include "cirDef.h"

extern CirMgr *cirMgr;

#define MAX_BUF_LEN 65536 //for getline char[] using

// TODO: Define your own data members and member functions
class CirMgr
{
  public:
    CirMgr()
    {
        CirGate *g = new CONST_gate(); // const 0 gate "一元復始，萬象更新"
        _gateList.push_back(g);
    }
    ~CirMgr() {}

    // Access functions
    // return '0' if "gid" corresponds to an undefined gate.
    CirGate *getGate(unsigned gid) const { return 0; }

    // Member functions about circuit construction
    bool readCircuit(const string &);

    // Member functions about circuit reporting
    void printSummary() const;
    void printNetlist() const;
    void printPIs() const;
    void printPOs() const;
    void printFloatGates() const;
    void writeAag(ostream &) const;

    bool myStr2Unsigned(const string &, unsigned &);

  private:
    unsigned _miloa[5];
    // M, maximum variable index
    // I, number of inputs
    // L, number of latches(not used in this homework)
    // O, number of outputs
    // A, number of AND gates

    // Arrays for Gates
    GateList _input;
    GateList _latch;
    GateList _output;
    GateList _aig;

    GateList _gateList;
};

#endif // CIR_MGR_H
