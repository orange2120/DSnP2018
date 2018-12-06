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

#include "cirDef.h"

extern CirMgr *cirMgr;

#define MAX_BUF_LEN 1024 //for getline char[] using

// TODO: Define your own data members and member functions
class CirMgr
{
  public:
    CirMgr() {}
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

  private:
    size_t mvi = 0;     // M, maximum variable index
    size_t in_num = 0;  // I, number of inputs
    size_t lac_num = 0; // L, number of latches(not used in this homework)
    size_t out_num = 0; // O, number of outputs
    size_t and_num = 0; // A, number of AND gates
};

#endif // CIR_MGR_H
