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
    IdList gatesToRm;
    // TODO
    for (unsigned i = 1, n = _gateList.size(); i < n; ++i)
    {
        if(_gateList[i] == NULL) continue;
        if(_gateList[i]->_typeID != PI_GATE && _gateList[i]->_typeID != PO_GATE)
        {
            if (_gateList[i]->_outList.empty())
            {
                gatesToRm.push_back(i);
            }
        }
    }
    // remove the unused AIG
    sort(gatesToRm.begin(), gatesToRm.end());
    for(unsigned i = 1, n = gatesToRm.size(); i < n; ++i)
    {
        CirGate *g = _gateList[gatesToRm[i]];
        CirGate *t = g->_fin1;

        t = g->_fin2;

        if(g->_typeID == AIG_GATE)
        {
            for(unsigned j = 0, n = _aig.size(); j<n; ++j)
            {
                if(_aig[i] == g->_id)
                    _aig.erase(_aig.begin() + j);
            }
        }
        else if(g->_typeID == PO_GATE)
        {
            for(unsigned j = 0, n = _output.size(); j<n; ++j)
            {
                if(_output[i] == g->_id)
                    _output.erase(_output.begin() + j);
            }
        }

        cout << "Sweeping: "<< g->_typeStr << '(' << g->_id << ") removed..." << endl;
        delete g;
        _gateList[gatesToRm[i]] = NULL;
        
        
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize()
{
    // TODO
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
