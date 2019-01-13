/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
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
}

void
CirMgr::fileSim(ifstream& patternFile)
{
    // TODO
    size_t lineNum = 0;
    string line;
    vector<bool> pattern;
    //vector<size_t> pattern;

    while (patternFile >> line)
    {
        lineNum++;
        if (line.size() > _miloa[1])
        {
            cerr << "Error: Pattern(" << line << ") length(" << line.size() << ") does not match the number of inputs(" << _miloa[1] << ") in a circuit!!" << endl;
            continue;
        }
        // read in input
        for (uint32_t i = 0; i < _miloa[1]; ++i)
        {
            pattern.push_back(line[i] - '0');
        }
    }
    
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
CirMgr::sim()
{

}