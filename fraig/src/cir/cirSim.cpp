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

//#define SIM_DEBUG_MSG

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
    size_t simulatedNum = 0;
    string line;
    vector<string> pattern;
    bool validPattern = true;

    // Perform simulation every 64 lines (bit)
    while (patternFile >> line)
    {
        lineNum++;
        if(!checkPattern(line))
        {
            validPattern = false;
            break;
        }

        pattern.push_back(line);
        if (lineNum % 64 == 0)
        {
            patternTrans(pattern);
            //printPIsimVal(); // DEBUG
            sim();
            writeSimlog(pattern);
            simulatedNum += 64;
            pattern.clear();
        }
    }
    if (lineNum % 64 != 0 && validPattern)
    {
        patternTrans(pattern, lineNum%64);
        //printPIsimVal(); // DEBUG
        sim();
        writeSimlog(pattern);
        simulatedNum = lineNum % 64;
    }

    cout << endl;
    cout << simulatedNum << " patterns simulated." << endl;
    patternFile.close();
}

void
CirMgr::writeSimlog(const vector<string> &pat)
{
    vector<string> outPattern(_output.size());
    getSimPO(outPattern, pat.size());
    for (unsigned i = 0, n = pat.size(); i < n; ++i)
    {
        *_simLog << pat[i] << ' ' << outPattern[i];
        *_simLog << endl;
    }
    
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
bool CirMgr::checkPattern(const string &str) const
{
    if (str.size() != _miloa[1])
    {
        cerr << endl;
        cerr << "Error: Pattern(" << str << ") length(" << str.size() << ") does not match the number of inputs(" << _miloa[1] << ") in a circuit!!" << endl;
        return false;
    }
    for (unsigned i = 0; i < str.size(); ++i)
    {
        if (str[i] != '0' && str[i] != '1')
        {
            cerr << endl;
            cerr << "Error: Pattern(" << str << ")  contains a non-0/1 character(‘"<< str[i] << "’)." << endl;
            return false;
        }
    }
    return true;
}

void
CirMgr::patternTrans(const vector<string> &pat, int n)
{
    // Reverse priority
    for (int i = n - 1; i >= 0; i--) 
    {
        for (unsigned j = 0; j < _input.size(); ++j)
        {
                // simulate PI gate
                _gateList[_input[j]]->_simVal = (_gateList[_input[j]]->_simVal << 1) | (size_t)(pat[i][j] - '0');
        }
    }
}

void
CirMgr::sim()
{
    unordered_map<size_t, IdList> simMap(getHashSize(_gateList.size() << 1));
    unordered_map<size_t, IdList>::iterator it;
    unsigned nFECGroup = 0;

    simAllGate();

    // add simulated gates into hash table
    for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        // Only need to handle AIG gates
        if (_dfsList[i]->_typeID != AIG_GATE) continue;

        // 找不到inv key and key
        if ((simMap.find(_dfsList[i]->_simVal) == simMap.end()) && (simMap.find(~_dfsList[i]->_simVal) == simMap.end()))
        {
            // insert positive phase ID
            simMap.insert(make_pair(_dfsList[i]->_simVal, IdList(1, (_dfsList[i]->_id << 1))));
        }
        else // key exists pos or inv
        {
            if ((it = simMap.find(_dfsList[i]->_simVal)) != simMap.end())
                it->second.push_back((_dfsList[i]->_id << 1));
            else if ((it = simMap.find(~_dfsList[i]->_simVal)) != simMap.end())
                it->second.push_back(((_dfsList[i]->_id << 1) + 1));
        }

        #ifdef SIM_DEBUG_MSG
            cerr << "Insert(" << _dfsList[i]->_id <<"): " << printBinSimVal(_dfsList[i]->_simVal) << " , " << printBinSimVal(~_dfsList[i]->_simVal) << endl;
        #endif
    }

    // Find buckets that has 2 data (or more), and they are FECs
    for (auto i = simMap.begin(); i != simMap.end(); ++i)
    {
        if (i->second.size() > 1)
            nFECGroup++;
    }
    //cerr << "nFEC.G:" << nFECGroup << endl;

    // FEC groups not found
    if (nFECGroup == 0)
        return;
    _fecs.resize(nFECGroup);

    #ifdef SIM_DEBUG_MSG
    for (auto i = simMap.begin(); i != simMap.end(); ++i)
    {
        if (i->second.size() > 1)
            for (unsigned j = 0; j < i->second.size(); ++j)
                cout << "G.ID: " << ((it->second[j] %2 == 0) ? it->second[j] >> 1: ((it->second[j] - 1) >> 1)) << " , G.SIMV: " << it->first << endl;
    }
    #endif

    unsigned k = 0;
    for (auto i = simMap.begin(); i != simMap.end(); ++i)
    {
        if (i->second.size() > 1)
        {
            for (unsigned j = 0; j < i->second.size(); ++j)
            {
                _fecs[k].push_back(i->second[j]);
            }
            k++;
        }
    }

    // Sort the FEC groups, and FECs
    for (unsigned i = 0; i < nFECGroup; ++i)
    {
        sort(_fecs[i].begin(), _fecs[i].end());
    }

    sort(_fecs.begin(), _fecs.end());
    cout << endl;
    cout << "Total #FEC Group = " << nFECGroup;
}

// simulate gates after PI (AIGs, POs)
void
CirMgr::simAllGate()
{
    for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        if (_dfsList[i]->_typeID != PO_GATE && _dfsList[i]->_typeID != AIG_GATE) continue;
        _dfsList[i]->simulation();
    }
}

// For log file output (n is sim amount)
void
CirMgr::getSimPO(vector<string> &out, int n)
{
    string output = "";
    size_t val = 0;
    for (uint8_t i = 0; i < n; ++i)
    {
        output = "";
        for (unsigned j = 0, m = _output.size(); j < m; ++j)
        {
            //val = (_gateList[_output[i]]->_simVal << i) & 1;
            output += to_string((_gateList[_output[j]]->_simVal << i) & 1);
        }
        out.push_back(output);
    }
}

// return vector index
vector<unsigned> *CirMgr::findFECs(const unsigned &gid) const
{
    for (unsigned i = 0, n = _fecs.size(); i < n; ++i)
    {
        if(binary_search(_fecs[i].begin(), _fecs[i].end(), gid << 1))
            return _fecs[i];
    }
    return NULL;
}

#ifdef SIM_DEBUG_MSG
/*************************************************/
/*        Private member for debugging           */
/*************************************************/
void
CirMgr::printPIsimVal() const
{
    for (uint32_t i = 0; i < _input.size(); ++i)
        cout << "P(" << _input[i] << "): " << "P.VAL: " << printBinSimVal(_gateList[_input[i]]->_simVal) << endl;
}

string
CirMgr::printBinSimVal(const size_t &val) const
{
    string str = "";
    for (int8_t i = 63; i >= 0; --i)
    {
        str += to_string((val >> i) & 1);
        if(i%8 == 0 && i > 0) str += '-';
    }
    return str;
}
#endif

// do dofiles/dosim