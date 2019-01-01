/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#include <cassert>
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
    IdList gateToRm;
    // TODO
    for (unsigned i = 1, n = _gateList.size(); i < n; ++i)
    {
        if(_gateList[i] == NULL) continue;
        if(_gateList[i]->_typeID != PI_GATE && _gateList[i]->_typeID != PO_GATE)
        {
            if (_gateList[i]->_outList.empty())
            {
                gateToRm.push_back(i);
            }
        }
    }
    // remove the unused AIG
    sort(gateToRm.begin(), gateToRm.end());
    for(unsigned i = 1, n = gateToRm.size(); i < n; ++i)
    {
        cirGate *g = gateList[gateToRm[j]];
        for(unsigned j = 0; j < g->_inList1.size(); ++j)
        {
            cirGate *t = g->_inList1[j];
            for(unsigned k = 0; k < t->_outList.size(); ++k)
            {
                if (t->_outList[k]->_id == g->_id)
                {
                    t->_outList[k].erase(t->_outList[k].begini() + i); 
                }
                    
            }
        }
        for(unsigned j = 0; j < g->_inList2.size(); ++j)
        {
            cirGate *t = g->_inList2[j];
            for(unsigned k = 0; k < t->_outList.size(); ++k)
            {
                if (t->_outList[k]->_id == g->_id)
                {
                    t->_outList[k].erase(t->_outList[k].begini() + i); 
                }
                    
            }
        }

        if(g->_typeID == AIG_GATE)
        {
            for(unsigned j = 0; n = _aig.size(); j<n; ++j)
            {
                if(_aig[i] == g->_id)
                    _aig.erase(_aig.begin() + j);
            }
        }
        else if(g->_typeID == AIG_GATE)
        {
            for(unsigned j = 0; n = _aig.size(); j<n; ++j)
            {
                if(_aig[i] == g->_id)
                    _aig.erase(_aig.begin() + j);
            }
        }

        cout << "Sweeping: "<< g->_typeStr << '(' << g->_id << ") removed..." << endl;
        delete g;
        gateList[gateToRm[i]] = NULL;
        
        
    }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void CirMgr::optimize()
{
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
