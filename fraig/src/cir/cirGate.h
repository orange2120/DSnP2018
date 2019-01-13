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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class CirGate
{
    friend class CirMgr;

  public:
    CirGate(unsigned &);
    virtual ~CirGate();

    // Basic access methods
    virtual string getTypeStr() const = 0;
    virtual uint8_t getTypeID() const { return _typeID; }
    unsigned getLineNo() const { return _lineNo; }
    void setLineNo(unsigned &ln) { _lineNo = ln + 1; }
    unsigned getID() const { return _id; }
    virtual bool isAig() const { return false; }

    void addFin1(CirGate *&);
    void addFin2(CirGate *&);
    void addFout(CirGate *&);
    CirGate *getFin1() const { return _fin[0]; }
    CirGate *getFin2() const { return _fin[1]; }
    void setInv1() { _inv[0] = true; }
    void setInv2() { _inv[1] = true; }
    bool IsInv1() const { return _inv[0]; }
    bool IsInv2() const { return _inv[1]; }
    virtual void setFin1(const unsigned &) {};
    virtual void setFin2(const unsigned &) {};
    void removeFiConn(unsigned &);
    void removeFiConn();
    void removeFoConn(unsigned &);
    void removeFoConn();
    void mergeIdentical();
    void mergeToGate(bool);
    void mergeToConst(CirGate *&);
    //void mergeGate(CirGate *&);
    void strMergeGate(CirGate *&);

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;

    // For DFS traversal
    void dfsTraversal(CirGate *, GateList &);
    void PrintFiDFS(const CirGate *, int &, int, bool) const;
    void PrintFoDFS(const CirGate *, int &, int, bool) const;
    void OptDFS(CirGate *, GateList &, IdList &);
    bool isGlobalRef() const { return (_ref == _globalRef); }
    void setToGlobalRef() const { _ref = _globalRef; }
    static void setGlobalRef() { _globalRef++; }

  private:
    unsigned _lineNo = 0;

    // For DFS traversal
    mutable unsigned _ref;
    static unsigned _globalRef;

  protected:
    unsigned _id; // Literal ID
    string _typeStr;
    uint8_t _typeID;
    string *_symbol;
    CirGate *_fin[2] = {NULL};
    GateList _outList;
    bool _inv[2] = {false};
};

class UNDEF_gate : public CirGate
{
  public:
    UNDEF_gate(unsigned &);
    ~UNDEF_gate() {}
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const;
    void setFin1(const unsigned &in) {};
    void setFin2(const unsigned &in) {};

  private:
};

class PI_gate : public CirGate
{
  public:
    PI_gate(unsigned);
    ~PI_gate() {}
    string getTypeStr() const { return "PI"; }
    void printGate() const;
    //void setFin1(const unsigned &in) {};
    void setFin2(const unsigned &in) {};

  private:
};

class PO_gate : public CirGate
{
  public:
    PO_gate(unsigned, unsigned &);
    ~PO_gate() {}
    void setInv(bool &i) { _inv[0] = i; }
    bool isInv() const { return _inv[0]; }
    string getTypeStr() const { return "PO"; }
    void printGate() const;
    void setFin1(const unsigned &in) { _in = in; };
    void setFin2(const unsigned &in) {};
    unsigned getIn() const { return _in; }

  private:
    unsigned _in;
};

class AIG_gate : public CirGate
{
  public:
    AIG_gate(unsigned, unsigned &, unsigned &);
    ~AIG_gate() {}
    bool isAig() const { return true; }
    string getTypeStr() const { return "AIG"; }
    void printGate() const;
    void setFin1(const unsigned &in) { _in1 = in; };
    void setFin2(const unsigned &in) { _in2 = in; };
    unsigned getIn1() const { return _in1; };
    unsigned getIn2() const { return _in2; }

  private:
    unsigned _in1, _in2;
};

class CONST_gate : public CirGate
{
  public:
    CONST_gate(unsigned);
    ~CONST_gate() {}
    string getTypeStr() const { return "CONST0"; }
    void printGate() const;
    void setFin1(const unsigned &in) {};
    void setFin2(const unsigned &in) {};

  private:
};

#endif // CIR_GATE_H
