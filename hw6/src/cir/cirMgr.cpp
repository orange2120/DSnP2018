/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr *cirMgr = 0;

enum CirParseError
{
    EXTRA_SPACE,
    MISSING_SPACE,
    ILLEGAL_WSPACE,
    ILLEGAL_NUM,
    ILLEGAL_IDENTIFIER,
    ILLEGAL_SYMBOL_TYPE,
    ILLEGAL_SYMBOL_NAME,
    MISSING_NUM,
    MISSING_IDENTIFIER,
    MISSING_NEWLINE,
    MISSING_DEF,
    CANNOT_INVERTED,
    MAX_LIT_ID,
    REDEF_GATE,
    REDEF_SYMBOLIC_NAME,
    REDEF_CONST,
    NUM_TOO_SMALL,
    NUM_TOO_BIG,

    DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0; // in printint, lineNo needs to ++
static unsigned colNo = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
    switch (err)
    {
    case EXTRA_SPACE:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Extra space character is detected!!" << endl;
        break;
    case MISSING_SPACE:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Missing space character!!" << endl;
        break;
    case ILLEGAL_WSPACE: // for non-space white space character
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Illegal white space char(" << errInt
             << ") is detected!!" << endl;
        break;
    case ILLEGAL_NUM:
        cerr << "[ERROR] Line " << lineNo + 1 << ": Illegal "
             << errMsg << "!!" << endl;
        break;
    case ILLEGAL_IDENTIFIER:
        cerr << "[ERROR] Line " << lineNo + 1 << ": Illegal identifier \""
             << errMsg << "\"!!" << endl;
        break;
    case ILLEGAL_SYMBOL_TYPE:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Illegal symbol type (" << errMsg << ")!!" << endl;
        break;
    case ILLEGAL_SYMBOL_NAME:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Symbolic name contains un-printable char(" << errInt
             << ")!!" << endl;
        break;
    case MISSING_NUM:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Missing " << errMsg << "!!" << endl;
        break;
    case MISSING_IDENTIFIER:
        cerr << "[ERROR] Line " << lineNo + 1 << ": Missing \""
             << errMsg << "\"!!" << endl;
        break;
    case MISSING_NEWLINE:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": A new line is expected here!!" << endl;
        break;
    case MISSING_DEF:
        cerr << "[ERROR] Line " << lineNo + 1 << ": Missing " << errMsg
             << " definition!!" << endl;
        break;
    case CANNOT_INVERTED:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": " << errMsg << " " << errInt << "(" << errInt / 2
             << ") cannot be inverted!!" << endl;
        break;
    case MAX_LIT_ID:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
             << endl;
        break;
    case REDEF_GATE:
        cerr << "[ERROR] Line " << lineNo + 1 << ": Literal \"" << errInt
             << "\" is redefined, previously defined as "
             << errGate->getTypeStr() << " in line " << errGate->getLineNo()
             << "!!" << endl;
        break;
    case REDEF_SYMBOLIC_NAME:
        cerr << "[ERROR] Line " << lineNo + 1 << ": Symbolic name for \""
             << errMsg << errInt << "\" is redefined!!" << endl;
        break;
    case REDEF_CONST:
        cerr << "[ERROR] Line " << lineNo + 1 << ", Col " << colNo + 1
             << ": Cannot redefine constant (" << errInt << ")!!" << endl;
        break;
    case NUM_TOO_SMALL:
        cerr << "[ERROR] Line " << lineNo + 1 << ": " << errMsg
             << " is too small (" << errInt << ")!!" << endl;
        break;
    case NUM_TOO_BIG:
        cerr << "[ERROR] Line " << lineNo + 1 << ": " << errMsg
             << " is too big (" << errInt << ")!!" << endl;
        break;
    default:
        break;
    }
    return false;
}

bool myStr2Unsigned(const string &str, unsigned &num)
{
    num = 0;
    size_t i = 0;
    int sign = 1;
    if (str[0] == '-')
    {
        sign = -1;
        i = 1;
    }
    bool valid = false;
    for (; i < str.size(); ++i)
    {
        if (isdigit(str[i]))
        {
            num *= 10;
            num += int(str[i] - '0');
            valid = true;
        }
        else
            return false;
    }
    num *= sign;
    return valid;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool CirMgr::readCircuit(const string &fileName)
{
    fstream f;
    f.open(fileName, ios::in);
    if (!f.is_open())
        return false;

    unsigned num = 0;
    char str[MAX_BUF_LEN];
    char *ptr;
    string s_str;
    vector<string> opts;

    // read in first line
    f.getline(str, MAX_BUF_LEN, '\n');

    cerr << str << endl;

    ptr = strtok(str, " ");
    while (ptr != NULL)
    {
        s_str.assign(ptr);
        opts.push_back(s_str);
        ptr = strtok(NULL, " ");
    }

    //cerr << "S:" << opts.size() << endl;

    for (size_t i = 0; i < opts.size(); i++)
    {
        cerr << opts[i] << endl;
    }

    if (opts[0] != "aag")
    {
        // TODO error message
        // Line 1: Illegal identifier
        return false;
    }

    // parse M I L O A
    for (unsigned i = 0; i < opts.size(); i++)
    {

        if (!myStr2Unsigned(opts[i], _miloa[i]))
        {
            // TODO error message invalid MILOA
            //[ERROR] Line 1: Illegal number of variables(
        }
    }

    // read in input
    if (_miloa[1] > 0)
    {
        for (size_t i = 0; i < _miloa[1]; i++)
        {
            f.getline(str, MAX_BUF_LEN, '\n');

            num = atoi(str);
            /*s_str.assign(str);

            if (!myStr2Int(str, num))
            {
            }*/
            // Create PI gate
            CirGate *g = new PI_gate(num);
            _gateList.push_back(g);
        }
    }

    // read in latch
    /*
    if (_miloa[2] > 0)
    {
        for (size_t i = 0; i < _miloa[2]; i++)
        {
            f.getline(str, MAX_BUF_LEN, '\n');
            s_str.assign(str);

            if (!myStr2Int(str, num))
            {
            }
            CirGate *g = new PI_gate(num);
            _gateList.push_back(g);
        }
    }*/

    // read in output
    if (_miloa[3] > 0)
    {
        unsigned n = _miloa[0] + 1;
        for (size_t i = 0; i < _miloa[5]; i++)
        {

            f.getline(str, MAX_BUF_LEN, '\n');
            num = atoi(str);
            /*s_str.assign(str);

            if (!myStr2Int(str, num))
            {
            }*/
            // Create PO gate
            n += i;
            CirGate *g = new PO_gate(n, num);
            _gateList.push_back(g);
            n = _miloa[0] + 1;
        }
    }

    // read in AIG gates
    if (_miloa[4] > 0)
    {
        unsigned t[3];
        // spilit
        for (size_t i = 0; i < _miloa[4]; i++)
        {
            f.getline(str, MAX_BUF_LEN, '\n');
            ptr = strtok(str, " ");
            t[0] = atoi(ptr);
            ptr = strtok(NULL, " ");
            t[1] = atoi(ptr);
            ptr = strtok(NULL, " ");
            t[2] = atoi(ptr);

            CirGate *g = new AIG_gate(t[0], t[1], t[2]);
            _gateList.push_back(g);
        }
    }

    // Optional symbol or comment
    while (f.getline(str, MAX_BUF_LEN, '\n'))
    {
        char c = str[0]; // extract first character
        char *ptr;
        str[0] = '\0';
        switch (c)
        {
        case 'i':
            /*
            ptr = strtok(str, " ");
            num = atoi(ptr);

            ptr = strtok(NULL, " ");
            t[1] = atoi(ptr);*/
            break;
        case 'o':
            break;
        case 'c':
            break;
        }
    }

    f.close();

    return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void CirMgr::printSummary() const
{
    cout << "Circuit Statistics" << endl;
    cout << "==================" << endl;

    cout << "------------------" << endl;
}

void CirMgr::printNetlist() const
{
}

void CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    cout << endl;
}

void CirMgr::printPOs() const
{
    cout << "POs of the circuit:";
    cout << endl;
}

void CirMgr::printFloatGates() const
{
}

void CirMgr::writeAag(ostream &outfile) const
{
    //TODO

    //outfile.close();
}
