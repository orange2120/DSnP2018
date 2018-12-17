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
class CirNet;
class CirPin;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes

class CirGate
{
  public:
    friend class CirMgr;
    // AIG
    CirGate(unsigned &);
    virtual ~CirGate();

    // Basic access methods
    virtual string getTypeStr() const = 0;
    virtual u_int8_t getTypeID() const { return _typeID; }
    unsigned getLineNo() const { return _lineNo; }
    void setLineNo(unsigned &ln) { _lineNo = ln + 1; }
    unsigned getID() const { return _id; }
    void addFin(CirGate *&);
    void addFin2(CirGate *&);
    void addFout(CirGate *&);

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;

    // For DFS traversal
    void dfsTraversal(CirGate *, GateList &);
    void PrintFiDFS(const CirGate *, int &, int, bool) const;
    void PrintFoDFS(const CirGate *, int &, int, bool) const;
    bool isGlobalRef() const { return (_ref == _globalRef); }
    void setToGlobalRef() const { _ref = _globalRef; }
    static void setGlobalRef() { _globalRef++; }

  private:
    unsigned _lineNo;

    // For DFS traversal
    mutable unsigned _ref;
    static unsigned _globalRef;

  protected:
    unsigned _id; // Literal ID
    string _typeStr;
    u_int8_t _typeID;
    string *_symbol;
    GateList _inList;
    GateList _inList2;
    GateList _outList;
    bool _inv1 = false;
    bool _inv2 = false;
};

class UNDEF_gate : public CirGate
{
  public:
    UNDEF_gate(unsigned &);
    ~UNDEF_gate(){}
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const;

  private:
};

class PI_gate : public CirGate
{
  public:
    PI_gate(unsigned &);
    ~PI_gate(){}
    string getTypeStr() const { return "PI"; }
    void printGate() const;

  private:
};

class PO_gate : public CirGate
{
  public:
    PO_gate(unsigned &, unsigned &);
    ~PO_gate(){}
    void setInv(bool &i) { _inv1 = i; }
    string getTypeStr() const { return "PO"; }
    void printGate() const;
    unsigned getIn() const { return _in; }

  private:
    unsigned _in;
};

class AIG_gate : public CirGate
{
  public:
    AIG_gate(unsigned &, unsigned &, unsigned &);
    ~AIG_gate(){}
    string getTypeStr() const { return "AIG"; }
    void printGate() const;
    unsigned getIn1() const { return _in1; };
    unsigned getIn2() const { return _in2; }

  private:
    unsigned _in1, _in2;
};

class CONST_gate : public CirGate
{
  public:
    CONST_gate(unsigned);
    ~CONST_gate(){}
    string getTypeStr() const { return "CONST0"; }
    void printGate() const;

  private:
};

#endif // CIR_GATE_H
