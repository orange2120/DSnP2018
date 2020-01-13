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
#include <bitset>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

//#define SIM_DEBUG_MSG
typedef unordered_map<size_t, IdList> SimMap;

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
    size_t simulatedNum = 0;
    size_t maxFail = _dfsList.size() + _miloa[1] - _miloa[3];
    vector<size_t> rnSim;
    

    if (maxFail > 50) maxFail = (size_t)(sqrt(maxFail)/4 > 50)? sqrt(maxFail)/4 : 50;
    cout << "MAX_FAILS = " << maxFail << endl;

    rnSim.reserve(maxFail);

    genRandom(rnSim);
    //constructFEC();
    if (_simLog != NULL)
        //writeSimlog();
    cout << simulatedNum << " patterns simulated." << endl;
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
    size_t nSimulated = 0;
    string line = "";
    vector<size_t> pattern(_miloa[1]);
    vector<string> input_pattern(64);
    bool validPattern = true;
    bool sim_d = false;

    // build FEC groups map
    _simMap.reserve(getHashSize(_miloa[0] << 4));

    cout << endl;

    // Perform simulation every 64 lines (bits)
    while (patternFile >> line)
    {

        input_pattern[lineNum % 64] = line;
        lineNum++;
        if(!checkPattern(line))
        {
            validPattern = false;
            break;
        }
        simPI(line);
        if (lineNum % 64 == 0)
        {
            patternTrans(pattern);
            simAllGate();
            constructFEC(sim_d);
            clearSim();
            nSimulated += 64;
            if (_simLog != NULL && validPattern)
                writeSimlog(input_pattern, lineNum);
            sim_d = true;

            input_pattern.clear();
            input_pattern.resize(64);
        }
    }

    // remain lines
    if(lineNum % 64 != 0 && validPattern)
    {
        patternTrans(pattern);
        simAllGate();
        constructFEC(sim_d);
        if (_simLog != NULL && validPattern)
           writeSimlog(input_pattern, lineNum);
        nSimulated += lineNum % 64;
    }

    sortFECs();

    cout << nSimulated << " patterns simulated." << endl;
    patternFile.close();
}

void
CirMgr::writeSimlog(const vector<string> &piPat, const size_t &lineNum)
{
    vector<size_t> poPattern(_miloa[3], 0);  // size = num of fanout
    vector<string> poStr(_miloa[3], "");
    uint8_t n = (lineNum % 64 == 0) ? 64 : lineNum % 64;

    // get fanout pattern
    for (unsigned i = 0; i < _miloa[3]; ++i)
        poPattern[i] = _gateList[_output[i]]->_simVal;
    for (unsigned i = 0; i < _miloa[3]; ++i)
        poStr[i] = bit2str(poPattern[i]);


    for (uint8_t i = 0; i < n; ++i)
    {
        *_simLog << piPat[i] << ' ';

        for (unsigned j = 0; j < _miloa[3]; ++j)
            *_simLog << poStr[j][i];
        *_simLog << endl;
    }
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
// Clear simulation results by setting PI to 0
void CirMgr::clearSim()
{
    for (unsigned i = 0; i < _miloa[1]; ++i)
        _gateList[_input[i]]->_simVal = 0;
    simAllGate();
}

void CirMgr::genRandom(vector<size_t> &rnVect)
{

}

// check if input pattern is valid
bool CirMgr::checkPattern(const string &str) const
{
    if (str.size() != _miloa[1])
    {
        cerr << "Error: Pattern(" << str << ") length(" << str.size() 
             << ") does not match the number of inputs(" << _miloa[1] << ") in a circuit!!\n";
        cerr << endl;
        return false;
    }
    for (unsigned i = 0; i < str.size(); ++i)
    {
        if (str[i] != '0' && str[i] != '1')
        {
            cerr << "Error: Pattern(" << str << ")  contains a non-0/1 character('"<< str[i] << "').\n";
            cerr << endl;
            return false;
        }
    }
    return true;
}

// Extract patterns from simulated PI gates 
inline void
CirMgr::patternTrans(vector<size_t> &pat)
{
    for (unsigned i = 0; i < _miloa[1]; ++i)
        pat[i] = _gateList[_input[i]]->_simVal;
}

// convert string to bit pattern and push into PI gate
void
CirMgr::simPI(const string &str)
{
    for (unsigned i = 0; i < str.size(); ++i)
    {
        _gateList[_input[i]]->_simVal += ((size_t)1 << _simCnt) * (size_t)(str[i] - '0');
    }
    if (_simCnt == 64)
    {
        _simCnt = 0;
        return;
    }
    _simCnt++;
}

// simulate all gates after PI (AIGs, POs)
void
CirMgr::simAllGate()
{
    for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
    {
        if (_dfsList[i]->_typeID != PO_GATE && _dfsList[i]->_typeID != AIG_GATE) continue;
        _dfsList[i]->simulation();
    }
}

void
CirMgr::constructFEC(bool dvi)
{
    SimMap::iterator group_it;
    IdList::iterator list_it;
    unsigned nFECGroup = 0;

    if (!dvi) // first time, add all the gates into a big FEC group
    {
        // Add CONST0
        _simMap.insert(make_pair(_gateList[0]->_simVal, IdList(1, 0)));

        // add simulated gates into hash table
        for (uint32_t i = 0, n = _dfsList.size(); i < n; ++i)
        {
            // Only need to handle AIG gates  
            if (_dfsList[i]->_typeID != AIG_GATE)
                continue;
            // query inverted key or key exists or not
            if ((_simMap.find(_dfsList[i]->_simVal) == _simMap.end()) && (_simMap.find(~_dfsList[i]->_simVal) == _simMap.end()))
            {
                // insert positive phase ID
                _simMap.insert(make_pair(_dfsList[i]->_simVal, IdList(1, (_dfsList[i]->_id << 1))));
                //cout << "N.K(" << _dfsList[i]->_id << ") S: " << _dfsList[i]->_simVal << " !S: " << ~_dfsList[i]->_simVal << endl;
            }
            else // key exists or inverted key exists
            {
                if ((group_it = _simMap.find(_dfsList[i]->_simVal)) != _simMap.end())
                {
                    group_it->second.push_back((_dfsList[i]->_id << 1)); // add the same key
                    //cout << "P.K(" << _dfsList[i]->_id << ") S: " << _dfsList[i]->_simVal << endl;
                }
                else if ((group_it = _simMap.find(~_dfsList[i]->_simVal)) != _simMap.end())
                {
                    group_it->second.push_back(((_dfsList[i]->_id << 1) + 1)); // add inverted key to list
                    //cout << "P.K(" << _dfsList[i]->_id << ") !S: " << ~_dfsList[i]->_simVal << endl;
                }
            }
        }
        // nFECGroup++;
    }
    else // divide FEC
    {
        for (auto i = _simMap.begin(); i != _simMap.end(); ++i) // in hash map
        {
            // size_t key = (*i).first;
            IdList *group = &(*i).second;

            for (auto g = group->begin(); g != group->end(); ++g) // in group
            {
                unsigned gid = (*g % 2 == 0) ? *g >> 1 : (*g - 1) >> 1;

                // gate exists in some group
                if ((group_it = _simMap.find(_gateList[gid]->_simVal)) != _simMap.end())
                {
                    (group_it)->second.push_back(gid << 1);
                    group->erase(g);
                }
                // if the fate not in current FEC groups, create a new one
                if (_simMap.find(_gateList[gid]->_simVal) == _simMap.end())
                {
                    _simMap.insert(make_pair(_gateList[gid]->_simVal, IdList(1, (gid << 1))));
                    // erase from current group
                    group->erase(g);

                    // nFECGroup++;
                }

            }

            /*
            // gid = (gid % 2 == 0) ? gid >> 1 : (gid - 1) >> 1; // first gate in a FEC group
            // if not in current FEC groups, create a new one
            if ((_simMap.find(_gateList[gid]->_simVal) == _simMap.end()) && (_simMap.find(~_gateList[gid]->_simVal) == _simMap.end()))
            {
                _simMap.insert(make_pair(_gateList[gid]->_simVal, IdList(1, (gid << 1))));
                // erase from original group
                
                nFECGroup++;
            }
            else // key or inverted key exists
            {
                // key exists
                if ((it = _simMap.find(_gateList[gid]->_simVal)) != _simMap.end())
                {
                    // copy old group to new one
                    for (unsigned j = 0; j < i->second.size(); ++j)
                    {
                        if(!binary_search(it->second.begin(),it->second.end(), i->second.operator[](j)))
                            it->second.push_back(i->second.operator[](j) << 1);
                    }
                }
                // inverted key exists
                else if ((it = _simMap.find(~_gateList[gid]->_simVal)) != _simMap.end())
                {
                    // copy old group to new one
                    for (unsigned j = 0; j < i->second.size(); ++j)
                    {
                        if(!binary_search(it->second.begin(),it->second.end(), i->second.operator[](j)))
                            it->second.push_back(((i->second.operator[](j) << 1) + 1));
                    }
                }
            }
            */
        }
    }

        #ifdef SIM_DEBUG_MSG
        for (auto i = _simMap.begin(); i != _simMap.end(); ++i)
        {
            if (i->second.size() > 1)
                for (unsigned j = 0; j < i->second.size(); ++j)
                    cout << "G.ID: " << ((it->second[j] %2 == 0) ? it->second[j] >> 1: ((it->second[j] - 1) >> 1)) << " , G.SIMV: " << it->first << endl;
        }
        #endif

        // traverse map to count FEC groups
        for (auto i = _simMap.begin(); i != _simMap.end(); ++i)
            nFECGroup++;

        cout << "Total #FEC Group = " << nFECGroup << endl;
    }

void CirMgr::divideFEC(SimMap &tmpSimMap)
{
    SimMap::iterator it;
    for (auto i = _simMap.begin(); i != _simMap.end(); ++i)
    {
        unsigned gid = i->second.operator[](0);
        gid = (gid % 2 == 0) ? gid >> 1 : (gid - 1) >> 1;
        // if not in new FEC groups, create a new one
        if ((tmpSimMap.find(_gateList[gid]->_simVal) == tmpSimMap.end()) && (tmpSimMap.find(~_gateList[gid]->_simVal) == tmpSimMap.end()))
        {
            tmpSimMap.insert(make_pair(_gateList[gid]->_simVal, IdList(1, (gid << 1))));
        }
        else // key or inverted key exists
        {
            if ((it = tmpSimMap.find(_gateList[gid]->_simVal)) != tmpSimMap.end())
            {
                // copy old group to new one
                for (unsigned j = 0; j < i->second.size(); ++j)
                {
                    if(!binary_search(it->second.begin(),it->second.end(), i->second.operator[](j)))
                        it->second.push_back(i->second.operator[](j) << 1);
                }
            }
            else if ((it = tmpSimMap.find(~_gateList[gid]->_simVal)) != tmpSimMap.end())
            {
                // copy old group to new one
                for (unsigned j = 0; j < i->second.size(); ++j)
                {
                    if(!binary_search(it->second.begin(),it->second.end(), i->second.operator[](j)))
                        it->second.push_back(((i->second.operator[](j) << 1) + 1));
                }
            }
        }
        
    }
    _simMap = tmpSimMap; // override old sim map
}

// convert bit pattern to string
inline string
CirMgr::bit2str(const size_t &pat)
{
    bitset<64> b(pat);
    string s = b.to_string();
    reverse(s.begin(), s.end());
    return s;
}
/*
inline size_t
CirMgr::reverseBits(const size_t &n)
{ 
    size_t rev = 0; 
    while (n > 0) 
    {
        rev <<= 1; 
        if (n & 1 == 1) 
            rev ^= 1; 
        n >>= 1; 
    } 
    return rev; 
}
*/

inline void
CirMgr::sortFECs()
{
    unsigned cnt = 0;
    for (auto i = _simMap.begin(); i != _simMap.end(); ++i)
    {
        sort(i->second.begin(), i->second.end());
        if (i->second.size() > 1)
        {
            _fecs.push_back(&i->second);
            cnt++;
        }
    }
    // Sort the FEC groups, and FECs
    sort(_fecs.begin(), _fecs.end(), compareIdList);
}

// find FECs
IdList *
CirMgr::findFECs(const size_t &simVal)
{
    SimMap::iterator it;
    if ((it = _simMap.find(simVal)) != _simMap.end())
        return &it->second;
    return NULL;
}

// compare by first element id IdList*
bool
CirMgr::compareIdList(const IdList *lhs, const IdList *rhs)
{
    return lhs->operator[](0) < rhs->operator[](0);
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
#endif

// print binary simulation values
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
