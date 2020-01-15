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
#include <unordered_map>

using namespace std;

//#define DEBUG_MSG

#define MAX_GATE_NUM 102400
#define MAX_BUF_LEN 65536 //for getline char[] using

//#define AAG_OPT_COMMENT "I ❤ DSnP"
#define AAG_OPT_COMMENT "AAG output by Chung-Yang (Ric) Huang"
#define AAG_GATE_OPT_COMMENT "I ❤ DSnP"

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirGate.h"
#include "cirDef.h"

extern CirMgr *cirMgr;

class CirMgr
{

  public:
    CirMgr();
    ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate *getGate(unsigned gid) const { return findGate(gid, _gateList); }
   CirGate *findGate(const unsigned &, const GateList &) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   //void writeSimlog(const vector<string> &,const size_t &);
   void writeSimlog(const vector<string> &, const size_t &);
   IdList *findFECs(const size_t &);

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   bool myStr2Unsigned(const string &, unsigned &);
   inline void IsValidID(CirMgr *, unsigned &);
   void IsRDEF(CirMgr *, unsigned &);
   bool IsUDEF(unsigned &);

   void buildConnection();
   void dfsTraversal(const IdList &);

 private:
   // Member functions about circuit optimization
   void removeGate(CirGate *&);
   void updateLists(IdList &);
   void OptDFS();

   // Member functions about simulation
   bool checkPattern(const string &) const;
   inline void patternTrans(vector<size_t> &);
   inline void getSimPO(vector<size_t> &);
   inline string bit2str(const size_t &pat);
   void simPI(const string &str);
   void simAllGate();
   void clearSim();
   void constructFEC(bool);
   void divideFEC(unordered_map<size_t, IdList> &);
   void genRandom(vector<size_t> &);
   void printPIsimVal() const;
   string printBinSimVal(const size_t &) const;
   static bool compareIdList(const IdList *, const IdList *);
   inline void sortFECs();
   inline size_t reverseBits(const size_t &);

   vector<IdList *> _fecs; // store pointer to reduce memory
   unordered_map<size_t, IdList> _simMap;
   ofstream *_simLog;
   size_t _simCnt = 0;

   // Member functions about fraig
   size_t finHashKey(CirGate *&);
   void strMergeGate(CirGate *&, CirGate *&);
   void genProofModel(SatSolver &);
   void reportResult(const SatSolver &, bool);
   void proofFECs(SatSolver&);

   // M, maximum variable index
   // I, number of inputs
   // L, number of latches(not used in this homework)
   // O, number of outputs
   // A, number of AND gates
   unsigned _miloa[5];

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
