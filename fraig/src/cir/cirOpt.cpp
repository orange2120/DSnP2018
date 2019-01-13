/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void CirMgr::sweep()
{
    IdList gatesToRm; // array to store the gates to be removed

    // create a table for _dfsList to compare with _gateList
    vector<bool> dfsL(_miloa[0] + _miloa[3] + 1, false);
    for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        dfsL[_dfsList[i]->_id] = true;
    }
    for(size_t i = 1, n = dfsL.size(); i < n; ++i)
    {
        if(_gateList[i] == NULL)
            continue;
        if (_gateList[i]->_typeID != PI_GATE && dfsL[i] == false)
            gatesToRm.push_back(i);
    }

    sort(gatesToRm.begin(), gatesToRm.end());

    // remove the unused gates
    
    for (size_t i = 0, n = gatesToRm.size(); i < n; ++i)
    {
        if (_gateList[gatesToRm[i]]->_typeID == AIG_GATE)
            for (size_t j = 0, k = _aig.size(); j < k; ++j)
            {
                if (_gateList[_aig[j]]->_id == gatesToRm[i])
                    _aig.erase(_aig.begin() + j);
            }

        if (_gateList[gatesToRm[i]]->_typeID == PO_GATE)
            for (size_t j = 0, k = _output.size(); j < k; ++j)
            {
                if (_gateList[_output[j]]->_id == gatesToRm[i])
                    _output.erase(_output.begin() + j);
            }
        SweepGate(_gateList[gatesToRm[i]]);
        _gateList[gatesToRm[i]] = NULL;
    }

    // Update MILOA
    _miloa[3] = _output.size();
    _miloa[4] = _aig.size();
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
//
// Fanin has constant 0 :  CONST0 ∧ a             ==> 0
// Fanin has constant 1 : !CONST0 ∧ a             ==> a
// Identical fanins :           a ∧ a or !a ∧ !a  ==> a or !a
// Inverted fanins :           !a ∧ a             ==> 0
void CirMgr::optimize()
{
    IdList gatesToRm;

    _dfsList.clear(); // clean _dfsList
    CirGate::setGlobalRef();
    for (unsigned i = 0, n = _input.size(); i < n; ++i)
    {
        if(_gateList[_input[i]]->_outList.empty()) continue;
        _gateList[_input[i]]->_outList[0]->OptDFS(_gateList[_input[i]]->_outList[0], _gateList, gatesToRm);
    }
    
    
    for (size_t i = 0, n = gatesToRm.size(); i < n; ++i)
    {
        if (_gateList[gatesToRm[i]]->_typeID == AIG_GATE)
            for (size_t j = 0, k = _aig.size(); j < k; ++j)
            {
                if (_gateList[_aig[j]]->_id == gatesToRm[i])
                    _aig.erase(_aig.begin() + j);
            }

        if (_gateList[gatesToRm[i]]->_typeID == PO_GATE)
            for (size_t j = 0, k = _output.size(); j < k; ++j)
            {
                if (_gateList[_output[j]]->_id == gatesToRm[i])
                    _output.erase(_output.begin() + j);
            }

        delete _gateList[gatesToRm[i]];
        _gateList[gatesToRm[i]] = NULL;
    }

    // Update MILOA
    _miloa[3] = _output.size();
    _miloa[4] = _aig.size();

    dfsTraversal(_output); // rebuild _dfsList
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void CirMgr::SweepGate(CirGate *&g)
{
    g->removeFiConn();
    g->removeFoConn();
    cout << "Sweeping: " << g->_typeStr << '(' << g->_id << ") removed..." << endl;
    delete g;
}

// Remove the gates that not exist in current ID lists
void CirMgr::updateLists(IdList &gatesToRm)
{
}