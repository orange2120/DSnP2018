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

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

// Initialize _globalRef
unsigned CirGate::_globalRef = 0;

CirGate::CirGate(unsigned &i) : _ref(0), _id(i)
{
    _symbol = NULL;
}

CirGate::~CirGate()
{
    if (_symbol != NULL)
        delete _symbol;
    _symbol = NULL;
    _fin[0] = NULL;
    _fin[1] = NULL;
}

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void CirGate::reportGate() const
{
    string str = "";
    IdList *gateFEC = NULL;
    bool isInvFEC = false;
    if ((gateFEC = cirMgr->findFECs(_simVal)) == 0)
    {
        gateFEC = cirMgr->findFECs(~_simVal);
        isInvFEC = true;
    }

    cout << "================================================================================" << endl;
    str = "= " + getTypeStr() + "(" + to_string(_id) + ")";
    if (_symbol != NULL)
        str += ("\"" + *_symbol + "\"");

    str += (", line " + to_string(_lineNo));
    cout << str << endl;

    str = "= FECs:";
    if (gateFEC != NULL && _typeID != PO_GATE && _typeID != PI_GATE)
    {
        bool phaseInv = false;
        for (unsigned i = 0, n = gateFEC->size(); i < n; ++i)
        {
            if (gateFEC->operator[](i) % 2 != 0)
               phaseInv = true;

            unsigned fec = (phaseInv) ? ((gateFEC->at(i) - 1) >> 1) : gateFEC->at(i) >> 1 ;
            if (fec != _id)
            {
                str += ' ';
                if (phaseInv ^ isInvFEC)
                    str += "!";
                str += to_string(fec);
            }
            phaseInv = false;
        }
    }

    cout << str << endl;
    str = "= Value: ";
    // TODO
    for (int8_t i = 63; i >= 0; --i)
    {
        str += to_string((_simVal >> i) & 1);
        if(i%8 == 0 && i > 0) str += '_';
    }

    cout << str << endl;
    cout << "================================================================================" << endl;
}

void CirGate::reportFanin(int level) const
{
    assert(level >= 0);
    setGlobalRef();
    printFiDFS(this, level, 0, 0);
}

void CirGate::reportFanout(int level) const
{
    assert(level >= 0);
    setGlobalRef();
    printFoDFS(this, level, 0, 0);
}

// add gate to fanin 1
void CirGate::addFin1(CirGate *&g)
{
    _fin[0] = g;
    g->_outList.push_back(this); // connect g's _outList to current gate
}

// add gate to fanin 2
void CirGate::addFin2(CirGate *&g)
{
    _fin[1] = g;
    g->_outList.push_back(this);
}

// add a gate to fanout gate list
void CirGate::addFout(CirGate *&g)
{
   _outList.push_back(g);
}

void CirGate::dfsTraversal(CirGate *node, GateList &l)
{
    if (_fin[0] != NULL)
    {
        if (!_fin[0]->isGlobalRef())
        {
            _fin[0]->setToGlobalRef();
            _fin[0]->dfsTraversal(_fin[0], l);
        }
    }
    if (_fin[1] != NULL)
    {
        if (!_fin[1]->isGlobalRef())
        {
            _fin[1]->setToGlobalRef();
            _fin[1]->dfsTraversal(_fin[1], l);
        }
    }
    l.push_back(node);
}

void CirGate::printFiDFS(const CirGate *node, int &level, int depth, bool inv) const
{
    assert(level >= 0);
    if (depth > level)
        return;

    for (int i = 0; i < depth; i++)
        cout << "  ";
    if (inv)
        cout << '!';
    cout << getTypeStr() << ' ' << _id;

    if (depth == level)
    {
        cout << endl;
            return;
    }

    if (isGlobalRef())
        cout << " (*)" << endl;
    else
    {
        cout << endl;

        if(_fin[0] != NULL)
            _fin[0]->printFiDFS(_fin[0], level, depth + 1, node->_inv[0]);
    
        if(_fin[1] != NULL)
            _fin[1]->printFiDFS(_fin[1], level, depth + 1, node->_inv[1]);

        if (_fin[0] != NULL || _fin[1] != NULL)
            setToGlobalRef();
    }
}

void CirGate::printFoDFS(const CirGate *node, int &level, int depth, bool inv) const
{
    assert(level >= 0);
    bool finv = false;
    if (depth > level)
        return;
    for (int i = 0; i < depth; i++)
        cout << "  ";
    if (inv)
        cout << '!';
    cout << getTypeStr() << " " << _id << endl;
    for (CirGate *g : _outList)
    {
        if (g->_inv[0] || g->_inv[1])
        {
            finv = true;
        }
        if (!g->isGlobalRef())
        {
            g->printFoDFS(g, level, depth + 1, finv);
            if (!(g->_outList.empty()))
                g->setToGlobalRef();
        }
        else
        {
            if (depth == level)
                return;
            for (int i = 0; i < depth + 1; i++)
                cout << "  ";
            if (inv)
                cout << "!";
            cout << g->getTypeStr() << " " << g->_id;
            if (depth < level - 1)
                cout << " (*)";
            cout << endl;
            return;
        }
    }
}

// Remove a specify gate from fanins
void CirGate::removeFiConn(unsigned &id)
{
    if (_fin[0] != NULL)
        if (_fin[0]->_id == id)
            _fin[0] = NULL;
    if (_fin[1] != NULL)
        if (_fin[1]->_id == id)
            _fin[1] = NULL;
}


// Let Fanin gates forget myself
void CirGate::removeFiConn()
{
    if (_fin[0] != NULL)
        for (unsigned i = 0; i < _fin[0]->_outList.size(); ++i)
        {
            if (_fin[0]->_outList[i] == this)
                _fin[0]->_outList.erase(_fin[0]->_outList.begin() + i);
        }
    if (_fin[1] != NULL)
        for (unsigned i = 0; i < _fin[1]->_outList.size(); ++i)
        {
            if (_fin[1]->_outList[i] == this)
                _fin[1]->_outList.erase(_fin[1]->_outList.begin() + i);
        }
}

// Remove a specified gate from a fanout gate list
void CirGate::removeFoConn(unsigned &id)
{
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        if (_outList[i]->_id == id)
            _outList.erase(_outList.begin() + i);
    }
}

// Let fanout gates delete .self
void CirGate::removeFoConn()
{
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        CirGate *g = _outList[i];
        if (g->_fin[0] == this)
            g->_fin[0] = NULL;
        if (g->_fin[1] == this)
            g->_fin[1] = NULL;
    }
}

// Merge identical gates together (_fin[0] == _fin[1])
void CirGate::mergeIdentical()
{
    // erase self from fanin gate's fanout list
    this->removeFiConn();

    cout << "Simplifying: " << _fin[0]->_id << " merging ";
    if(_inv[0]) cout << '!';

    // for each gate in fanout list
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        // connect next to prev
        // self are connected to fin1
        
        if (_outList[i]->_fin[0] == this)
        {
            _outList[i]->_fin[0] = this->_fin[0];
            _outList[i]->_inv[0] = _inv[0] ^ _outList[i]->_inv[0];
            _outList[i]->setFin1(this->_fin[0]->_id);
        }
        // self are connected to fin2
        else
        {
            _outList[i]->_fin[1] = this->_fin[1];
            _outList[i]->_inv[1] = _inv[1] ^ _outList[i]->_inv[1];
            _outList[i]->setFin2(this->_fin[1]->_id);
        }
        // connect prev to next
        _fin[0]->_outList.push_back(_outList[i]);
    }
    cout << _id << "...";
}

// Merge gates to a gate, the input gate is used to replace current gate.
void CirGate::mergeToGate(bool in)
{
    // gate to merge to next gate. False represents fanin 1 (CONST at fanin 1), true is fanin 2
    CirGate *g = (in) ? _fin[1] : _fin[0];
    // delete self from fanout's fanin
    this->removeFiConn();

    cout << "Simplifying: " << g->_id << " merging ";
    if(_inv[in]) cout << '!';

    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        // connect next to prev
        if (_outList[i]->_fin[0] == this)
        {
            if(_inv[in])
                _outList[i]->_inv[0] = (_inv[in] ^ _outList[i]->_inv[0]);
            // connect fanouts to g
            _outList[i]->_fin[0] = g;
            _outList[i]->setFin1(g->_id);
        }
        else
        {
            if(_inv[in])
                _outList[i]->_inv[1] = (_inv[in] ^ _outList[i]->_inv[1]);
            _outList[i]->_fin[1] = g;
            _outList[i]->setFin2(g->_id);
        }
        // connect prev to next
        g->_outList.push_back(_outList[i]);
    }
    cout << _id << "...";
}

// Merging gate to CONST gate (pass in CONST gate)
void CirGate::mergeToConst(CirGate *&g)
{
    this->removeFiConn();

    cout << "Simplifying: " << g->_id << " merging ";
    for (unsigned i = 0; i < _outList.size(); ++i)
    {
        if (_outList[i]->_fin[0] == this)
        {
            _outList[i]->_fin[0] = g;
            _outList[i]->setFin1(g->_id);
        }
        else
        {
            _outList[i]->_fin[1] = g;
            _outList[i]->setFin2(g->_id);
        }

        g->_outList.push_back(_outList[i]);
    }
    cout << _id << "...";
}

// Merge gate together, (gate to be merged, gate to merge)
void CirGate::strMergeGate(CirGate *&prev)
{
    for (unsigned i = 0; i < prev->_outList.size(); ++i)
    {
        this->_outList.push_back(prev->_outList[i]);

        // Replace fanin by self
        if(prev->_outList[i]->_fin[0] == prev)
        {
            prev->_outList[i]->_fin[0] = this;
            prev->_outList[i]->setFin1(this->_id);
        }
        else
        {
            prev->_outList[i]->_fin[1] = this;
            prev->_outList[i]->setFin2(this->_id);
        }
    }
}

// simulation
// IMPORTANT: Consider PHASE inverting
void CirGate::simulation()
{
    size_t f1 = _fin[0]->_simVal;
    size_t f2 = _fin[1]->_simVal;
    _simVal = ((_inv[0]) ? ~f1 : f1) & ((_inv[1]) ? ~f2 : f2);
}

/**************************************/
/* class UNDEF GATE member functions  */
/**************************************/
UNDEF_gate::UNDEF_gate(unsigned &n) : CirGate(n)
{
    _typeID = UNDEF_GATE;
}

void UNDEF_gate::printGate() const
{
    cout << "UNDEF " << _id;
}

/**************************************/
/*   class AIG GATE member functions  */
/**************************************/

AIG_gate::AIG_gate(unsigned n, unsigned &i1, unsigned &i2) : CirGate(n), _in1(i1), _in2(i2)
{
    _typeID = AIG_GATE;

    if (_in1 % 2 != 0)
    {
        _in1--;
        _inv[0] = true;
    }
    _in1 >>= 1;

    if (_in2 % 2 != 0)
    {
        _in2--;
        _inv[1] = true;
    }
    _in2 >>= 1;
}

void AIG_gate::printGate() const
{
    cout << "AIG " << _id << ' ';
    if (_inv[0])
        cout << '!';
    cout << _in1 << ' ';
    if (_inv[1])
        cout << '!';
    cout << _in2 << endl;
}

/**************************************/
/*   class PI GATE member functions   */
/**************************************/
PI_gate::PI_gate(unsigned n) : CirGate(n)
{
   _typeID = PI_GATE;
}

void PI_gate::printGate() const
{
    cout << "PI  " << _id;
}

/**************************************/
/*   class PO GATE member functions   */
/**************************************/
PO_gate::PO_gate(unsigned n, unsigned &i) : CirGate(n), _in(i)
{
   _typeID = PO_GATE;
   if (_in % 2 != 0)
   {
      _in--;
      _inv[0] = true;
   }
   _in >>=  1;
}

void PO_gate::printGate() const
{
    cout << "PO  " << _id << " ";
    if (_inv[0])
        cout << '!';
    cout << _in;
}

/**************************************/
/* class CONST GATE member functions  */
/**************************************/
CONST_gate::CONST_gate(unsigned n = 0) : CirGate(n)
{
   _typeID = CONST_GATE;
}

void CONST_gate::printGate() const
{
   cout << "CONST0";
}
