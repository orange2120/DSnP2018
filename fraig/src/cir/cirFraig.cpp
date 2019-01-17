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

    unordered_map<size_t, CirGate *> faninMap(getHashSize(_miloa[0] << 2));
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
        
        if((it = faninMap.find(gp.first)) != faninMap.end())
            strMergeGate(_dfsList[i], it->second);
        else
            faninMap.insert(gp);
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
    IdList gatesToMerge;
    SatSolver solver;
    solver.initialize();

    genProofModel(solver);

    //For each FEC pair
    proofFECs(solver);

    //fraigMerge(gatesToMerge);

    _dfsList.clear();
    dfsTraversal(_output); // rebuild _dfsList
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

// construct all the gates
void CirMgr::genProofModel(SatSolver& s)
{
    CirGate *g = _gateList[0];
    // IMPORTANT: CONST0 must be included

    // TODO 做一個假的CONST0，其實是AIG
    //CirGate *constAIG = new CirGate::AIG_gate(0);
    //s.addAigCNF(g->getVar(), g->_fin[0]->getVar(), g->_inv[0],g->_fin[1]->getVar(), g->_inv[1]);

    for (unsigned i = 0, n = _dfsList.size(); i < n; ++i)
    {
        if (_dfsList[i]->_typeID == PO_GATE || _dfsList[i]->_typeID == CONST_GATE)
            continue;
        g = _dfsList[i];

        Var v = s.newVar();
        _dfsList[i]->setVar(v);
        s.addAigCNF(g->getVar(), g->_fin[0]->getVar(), g->_inv[0],
            g->_fin[1]->getVar(), g->_inv[1]);
    }
}

void CirMgr::reportResult(const SatSolver& solver, bool result)
{
   solver.printStats();
   cout << (result? "SAT" : "UNSAT") << endl;
   if (result) {
      for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
         cout << solver.getValue(_dfsList[i]->getVar()) << endl;
   }
}

void CirMgr::proofFECs(SatSolver& s)
{
    Var newV = s.newVar();
    
    bool result;

    for (unsigned i = 0, n = _fecs.size(); i < n; ++i)
    {
        for (unsigned j = 0, m = _fecs[i]->size(); j < m; ++j) // 
        {
            for (unsigned k = 0; k < j; ++k)
            {
                newV = s.newVar();
                //s.addXorCNF(newV, _fecs[i]->operator[](j)->getVar(), _fecs[j]->_inv[0], _fecs[k]->getVar(), _fecs[k]->_inv[1]);
                s.assumeRelease();  // Clear assumptions
                s.assumeProperty(newV, true);  // k = 1
                result = s.assumpSolve();
                reportResult(s, result);
            }
        }
    }
}
