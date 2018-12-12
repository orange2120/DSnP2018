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

#define MAX_SYMBOL_LEN 512

class CirGate;
class CirNet;
class CirPin;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes

// Circuit net list
// Pins will connectn to net list
/*
class CirNet
{
    //vector<CirPin *> _inPinList;
    //vector<CirPin *> _outPinList;
};
*/

// Define a pin object
class CirPin
{
    friend class CirGate;
    friend class CirMgr;
    //CirPin(CirGate *p) : _gate(p) {}
    //~CirPin();
    CirGate *_gate;

    // input or output
    vector<CirGate *> _io;
    //CirNet *_net;
};

class CirGate
{
  public:
    friend class CirMgr;
    // AIG
    CirGate(unsigned &);
    virtual ~CirGate();

    // Basic access methods
    string getTypeStr() const { return ""; }
    unsigned getLineNo() const { return _lineNo; }

    // Printing functions
    virtual void printGate() const = 0;
    void reportGate() const;
    void reportFanin(int level) const;
    void reportFanout(int level) const;

    //void add(GateType);

  private:
    unsigned _id; // Literal ID
    size_t _lineNo;
    string _typeStr;
    // TODO 改用pointer
    char _symbol[MAX_SYMBOL_LEN];
    //vector<CirGate *> _inList;
    //vector<CirGate *> _outList;
    //vector<CirPin *> _pins;

  protected:
};

class AIG_gate : public CirGate
{
  public:
    AIG_gate(unsigned &, unsigned &, unsigned &);
    ~AIG_gate();
    string getTypeStr() const { return "AIG"; }
    void printGate() const;

  private:
    bool _inv1 = false;
    bool _inv2 = false;
    unsigned _in1, _in2;
    CirPin *_fin1;
    CirPin *_fin2;
    CirPin *_fout;
};

class PI_gate : public CirGate
{
  public:
    PI_gate(unsigned &);
    ~PI_gate();
    string getTypeStr() const { return "PI"; }
    void printGate() const;

  private:
    unsigned _out;
    CirPin *_fout;
};

class PO_gate : public CirGate
{
  public:
    PO_gate(unsigned &, unsigned &);
    ~PO_gate();
    void setInv(bool &i) { _inv = i; }
    string getTypeStr() const { return "PO"; }
    void printGate() const;

  private:
    unsigned _in;
    CirPin *_fin;
    bool _inv = false;
};

class CONST_gate : public CirGate
{
  public:
    CONST_gate(unsigned);
    ~CONST_gate();
    string getTypeStr() const { return "CONST0"; }
    void printGate() const;

  private:
    CirPin *_fout;
};

class UNDEF_gate : public CirGate
{
  public:
    UNDEF_gate(unsigned &);
    ~UNDEF_gate();
    string getTypeStr() const { return "UNDEF"; }
    void printGate() const;

  private:
    CirPin *_fin;
};

#endif // CIR_GATE_H
