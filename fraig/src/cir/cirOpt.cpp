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

//#define PRINT_TYPE

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
        cout << "Sweeping: " << _gateList[gatesToRm[i]]->_typeStr << '(' << _gateList[gatesToRm[i]]->_id << ") removed..." << endl;
        removeGate(_gateList[gatesToRm[i]]);
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
    OptDFS();

    // Update MILOA
    _miloa[3] = _output.size();
    _miloa[4] = _aig.size();

    _dfsList.clear();
    dfsTraversal(_output); // rebuild _dfsList
}

//do dofiles/dos09
void CirMgr::OptDFS()
{
    IdList gatesToRm;
    bool gateTodel = false;

    for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        CirGate *g = _dfsList[i];

        // PI have no effective fanin
        if (g->_typeID == PI_GATE || g->_typeID == CONST_GATE)
            continue;

        // if current gate is PO_GATE, NO NEED TO MERGE
        if (g->_typeID == PO_GATE)
            continue;

        // Fanin has constant 0 or 1
        if (g->_fin[0]->_typeID == CONST_GATE)
        {
            // Replaced by the other fanin (fanin 2)
            if(g->_inv[0])
            {
                // fanin 2, non inverted
                g->mergeToGate(true);
            #ifdef PRINT_TYPE
                cout << " (Fanin has constant 1)" << endl;
            #else
                cout << endl;
            #endif
            }
            // Fanin 1 has constant 0, replaced by 0
            else
            {
                g->mergeToGate(false);
            #ifdef PRINT_TYPE
                cout << " (Fanin has constant 0)" << endl;
            #else
                cout << endl;
            #endif
            }
            gateTodel = true;
        }
        else if (g->_fin[1]->_typeID == CONST_GATE)
        {
            // Replaced by fanin 1
            if(g->_inv[1])
            {
                g->mergeToGate(false);
            #ifdef PRINT_TYPE
                cout << " (Fanin has constant 1)" << endl;
            #else
                cout << endl;
            #endif
            }
            else
            {
                g->mergeToGate(true);
            #ifdef PRINT_TYPE
                cout << " (Fanin has constant 0)" << endl;
            #else
                cout << endl;
            #endif
            }
            gateTodel = true;
        }
        // Identical fanins
        else if (g->_fin[0] == g->_fin[1])
        {
            // Inverted fanins
            if(g->_inv[0] != g->_inv[1])
            {
                g->mergeToConst(_gateList[0]);
            #ifdef PRINT_TYPE
                cout << " (Inverted fanins)" << endl;
            #else
                cout << endl;
            #endif
            }
            else
            {
                g->mergeIdentical();
            #ifdef PRINT_TYPE
                cout << " (Identical fanins)" << endl;
            #else
                cout << endl;
            #endif
            }
            gateTodel = true;
        }

        if(gateTodel)
            gatesToRm.push_back(g->_id);
        gateTodel = false;
    }

    // TODO 改掉
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
        removeGate(_gateList[gatesToRm[i]]);
    }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
void CirMgr::removeGate(CirGate *&g)
{
    g->removeFiConn();
    g->removeFoConn();
    _gateList[g->_id] = NULL;
    delete g;
}

// Remove the gates that not exist in current ID lists
void CirMgr::updateLists(IdList &gatesToRm)
{
}