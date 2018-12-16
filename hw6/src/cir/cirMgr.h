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

//#define DEBUG_MSG

#define MAX_GATE_NUM 102400
#define MAX_BUF_LEN 65536 //for getline char[] using

//#define AAG_OPT_COMMENT "I ❤ DSnP"
#define AAG_OPT_COMMENT "AAG output by Chung-Yang (Ric) Huang"

// TODO: Define your own data members and member functions
class CirMgr
{
  friend class CirGate;

public:
  CirMgr()
  {
    // initialize Gate list array
    CirGate *g = new CONST_gate(0); // const 0 gate "一元復始，萬象更新"
    _gateList.push_back(g);
    _gateListIdx.push_back(0);
    _gateListSize++;
  }
  ~CirMgr() {}

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate *getGate(unsigned gid) const { return findGate(gid, _gateList); }
  CirGate *findGate(const unsigned &, const GateList &) const;

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

  void buildConnection();
  void dfsTraversal(const GateList &);

private:
  unsigned _miloa[5];
  // M, maximum variable index
  // I, number of inputs
  // L, number of latches(not used in this homework)
  // O, number of outputs
  // A, number of AND gates

  size_t _gateListSize = 0;
  // Arrays for Gates
  GateList _input;
  GateList _latch;
  GateList _output;
  GateList _aig;
  GateList _undef;

  GateList _gateList;
  IdList _gateListIdx;
  GateList _dfsList;

  // comment for the aag file
  vector<string> _comments;
};

#endif // CIR_MGR_H
