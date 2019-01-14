/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#include <cassert>
#include <unordered_map>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/


/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{

    unordered_map<size_t, CirGate *> _faninMap(getHashSize(_dfsList.size()));
    unordered_map<size_t, CirGate *>::iterator it;
    // TODO
    // For each gate, create hash table <fanins, this gate>
    // Perform strash only on gates in DFS list

    // WARNING: After merging, hashkey WILL CHANGE. handle hashkey updating.
    for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        // strash only effective on AIG gate
        if(!_dfsList[i]->isAig()) continue;

        pair<size_t, CirGate *> gp(finHashKey(_dfsList[i]), _dfsList[i]);
        
        if((it = _faninMap.find(gp.first)) != _faninMap.end())
            strMergeGate(_dfsList[i], it->second);
        else
            _faninMap.insert(gp);
    }
    // HashKey depends on gatetype & list of fanins

    // TODO 改善更新 ID list效率
    // 改用重新建構

    // Update MILOA
    _miloa[4] = _aig.size();

    _dfsList.clear();
    dfsTraversal(_output); // rebuild _dfsList
}

void
CirMgr::fraig()
{
    // TODO
    SatSolver solver;
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/

void CirMgr::strMergeGate(CirGate *&prev, CirGate *&curr)
{
    cout << "Strashing: "<< curr->_id << " merging " << prev->_id << "...\n";
    curr->strMergeGate(prev);

    // TODO 改掉
    for (size_t j = 0, k = _aig.size(); j < k; ++j)
    {
        if (_gateList[_aig[j]]->_id == prev->_id)
            _aig.erase(_aig.begin() + j);
    }

    prev->removeFoConn();
    prev->removeFiConn();
    _gateList[prev->_id] = NULL;
    delete prev;
}

size_t CirMgr::finHashKey(CirGate *&g)
{
    size_t a = 0;
    size_t b = 0;
    bool sel = false;

    if (g->_fin[1] < g->_fin[0]) sel = true; // swap fanin 1, fanin 2
    a = g->_fin[sel]->_id;
    b = g->_fin[!sel]->_id;

    return (a << 32) + (b << 2) + (g->_inv[!sel] << 1) + g->_inv[sel];
}
// do dofiles/dostr
