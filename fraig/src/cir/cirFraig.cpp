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
/*
class HashKey
{
  public:
    HashKey(const CirGate *g)
    {
        uint32_t a = g->getFin1()->getID() + g->IsInv1();
        uint32_t b = g->getFin2()->getID() + g->IsInv2();
        for (size_t i = 0; i < 5; ++i)
            _key ^= ((a + b) << (i * 6));
    }
    size_t operator()() const { return _key; }
    bool operator==(const HashKey &k) const { return (_key == k._key); }
    HashKey operator=(const HashKey &h) { _key = h._key; return *(this); }

  private:
    size_t _key = 0;
};
*/
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

    unordered_map<size_t, CirGate *> _faninMap; //(getHashSize(_dfsList.size()));
    unordered_map<size_t, CirGate *>::iterator it;
    // TODO
    // For each gate, create hash table <fanins, this gate>
    // Perform strash only on gates in DFS list

    // WARNING: After merging, hashkey WILL CHANGE. handle hashkey updating.
    for (size_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        // strash only effective on AIG gate
        if(!_dfsList[i]->isAig()) continue;

        pair<size_t, CirGate *> gp(HashKey(_dfsList[i]), _dfsList[i]);
        
        if((it = _faninMap.find(gp.first)) != _faninMap.end())
        {
            //_faninMap.erase()
            strMergeGate(it->second, _dfsList[i]);
        }
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
    // TODO 把prev的out變成自己的，接到對面
    cout << "Strashing: "<< prev->_id << " merging " << curr->_id << "...\n";

    curr->strMergeGate(prev);

    // TODO 改掉
    for (size_t j = 0, k = _aig.size(); j < k; ++j)
    {
        if (_gateList[_aig[j]]->_id == prev->_id)
            _aig.erase(_aig.begin() + j);
    }

    prev->removeFoConn();
    prev->removeFiConn();
    _gateList[prev->_id] = 0;
    delete prev;
}

size_t CirMgr::HashKey(CirGate *&g)
{
    size_t key = 0;
    uint32_t a = g->_fin[0]->_id + g->_inv[0];
    uint32_t b = g->_fin[1]->_id + g->_inv[1];
    for (size_t i = 0; i < 5; ++i)
        key ^= ((a + b) << (i * 6));
    return key;
}
// do dofiles/dostr