/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <unordered_map>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    // TODO
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    // TODO
    // 010
    // 101
    // 110
    // ^ 011.... Parallel simulation pattern
    size_t lineNum = 0;
    string line;
    vector<bool> pattern;

    unsigned times = 0;

    while (patternFile >> line)
    {
        lineNum++;
        cout << line << endl;

        if (line.size() != _miloa[1])
        {
            cerr << "Error: Pattern(" << line << ") length(" << line.size() << ") does not match the number of inputs(" << _miloa[1] << ") in a circuit!!" << endl;
            continue;
        }
        for (unsigned i = 0; i < line.size(); ++i)
        {
            if (line[i] != '0' && line[i] != '1')
            {
                cerr << "Error: Pattern(" << line << ")  contains a non-0/1 character(‘"<< line[i] << "’)." << endl;
                continue;
            }
        }
        /*
        // read in input
        for (uint32_t i = 0; i < _input.size(); ++i)
        {
            // simulate PI gate
            _gateList[_input[i]]->_simVal = (_gateList[_input[i]]->_simVal) << 1;
            _gateList[_input[i]]->_simVal += line[i] - '0';
            //times++;
        }
        */
    }
    //cout << "LineNum: " << lineNum << endl;
    //cout << "Times: " << times << endl;
    /*for (uint32_t i = 0; i < _input.size(); ++i)
        {
            // simulate PI gate
            _gateList[_input[i]]->_simVal = ~_gateList[_input[i]]->_simVal;
        }*/


    //*** DEBUG ***
    for (uint32_t i = 0; i < _input.size(); ++i)
    {
        string str = "";
        cout << "P(" << _input[i] << "): " << _gateList[_input[i]]->_simVal << endl;
        for (int j = 63; j >= 0; --j)
        {
            str += to_string((_gateList[_input[i]]->_simVal >> j) & 1);
            if(j%8 == 0 && j > 0) str += '_';
        }
        cout << "PVAL: " << str << endl;
    }

    sim();

    cout << lineNum << " patterns simulated." << endl;
    patternFile.close();
}

void
CirMgr::writeSimlog()
{

}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::patternTrans(vector<bool> &p, unsigned &n)
{
    // simulate PI
    for (uint32_t i = 0; i < p.size(); ++i)
    {
        // simulate PI gate
        _gateList[n]->_simVal = ((_gateList[n]->_simVal) << 1) + p[i];
    }
}

void
CirMgr::sim()
{
    unordered_map<size_t, unsigned> simMap;
    unsigned nbuckets = 0;
    unsigned nFECGroup = 0;

    simAllGate();

    // add simulated gates into hash table
    for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        // Only need to handle AIG gates
        if (_dfsList[i]->_typeID != AIG_GATE) continue;
        simMap.insert({_dfsList[i]->_simVal, _dfsList[i]->_id});
    }

    nbuckets = simMap.bucket_count();

    // Find buckets that has 2 data (or more), and they are FECs
    for (unsigned i = 0; i < nbuckets; ++i)
    {
        //cerr << simMap.bucket_size(i) << endl;
        if (simMap.bucket_size(i) > 1)
            nFECGroup++;
    }
    //cout << "NFC:" << nFECGroup << endl;

    /*
    for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        
    }*/

    // FEC groups not found
    if (nFECGroup == 0)
        return;
    _fecs.reserve(nFECGroup);

    for (unsigned i = 0; i < nFECGroup; ++i)
    {
        if(simMap.bucket_size(i) > 1)
        {
            for (auto it = simMap.begin(i); it != simMap.end(i); ++it)
                cout << "SIMV:" << it->first << ",ID:" << it->second << endl;
        }
        cout << endl;
    }

    for (unsigned i = 0; i < nFECGroup; ++i)
    {
        if(simMap.bucket_size(i) > 1)
        {
            for (auto it = simMap.begin(i); it != simMap.end(i); ++it)
                _fecs[i].push_back(it->second);
        }
    }

    // Sort the FEC groups, and FECs
    for (unsigned i = 0; i < nbuckets; ++i)
    {
        sort(_fecs[i].begin(), _fecs[i].end());
    }
    sort(_fecs.begin(), _fecs.end());


}

// simulate gates after PI (AIGs, POs)
void
CirMgr::simAllGate()
{
    // simulate AIG and PO gate
    // IMPORTANT: Consider PHASE
    for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        if(_dfsList[i]->_typeID == PO_GATE)
            _dfsList[i]->_simVal = _dfsList[i]->_fin[0]->_simVal ^ _dfsList[i]->_inv[0];
        else if (_dfsList[i]->_typeID == AIG_GATE)
            _dfsList[i]->_simVal = (_dfsList[i]->_fin[0]->_simVal ^ _dfsList[i]->_inv[0]) & (_dfsList[i]->_fin[1]->_simVal ^ _dfsList[i]->_inv[1]);
    }
}
/*
void
CirMgr::simHash()
*/
// do dofiles/dosim