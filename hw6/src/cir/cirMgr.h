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

public:
  CirMgr();
  ~CirMgr();

  // Access functions
  // return '0' if "gid" corresponds to an undefined gate.
  CirGate *getGate(unsigned gid) const
  {
    return findGate(gid, _gateList);
  }
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
  inline void IsValidID(CirMgr *, unsigned &);
  void IsRDEF(CirMgr *, unsigned &);

  void buildConnection();
  void dfsTraversal(const IdList &);

private:
  unsigned _miloa[5];
  // M, maximum variable index
  // I, number of inputs
  // L, number of latches(not used in this homework)
  // O, number of outputs
  // A, number of AND gates

  // Arrays for Gates
  IdList _input;
  IdList _latch;
  IdList _output;
  IdList _aig;

  GateList _gateList;
  GateList _dfsList;
  
  // comment for the aag file
  vector<string> _comments;
};

#endif // CIR_MGR_H
