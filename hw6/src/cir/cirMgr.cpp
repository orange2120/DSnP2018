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
#include <algorithm>
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

bool CirMgr::myStr2Unsigned(const string &str, unsigned &num)
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
    {
        cerr << "Cannot open design \"" << fileName << "\"!!" << endl;
        return false;
    }

    unsigned num = 0;
    char str[MAX_BUF_LEN];
    char *ptr;
    string s_str;
    vector<string> opts;
    bool read_status = true;

    // read in first line
    f.getline(str, MAX_BUF_LEN, '\n');

#ifdef DEBUG_MSG
    cerr << "Reading..." << endl;
    cerr << str << endl
         << endl;
#endif

    try
    {
        ptr = strtok(str, " ");
        while (ptr != NULL)
        {
            s_str.assign(ptr);
            opts.push_back(s_str);
            ptr = strtok(NULL, " ");
        }

        if (opts[0] != "aag")
        {
            errMsg = opts[0];
            throw ILLEGAL_IDENTIFIER;
        }

        // parse M I L O A
        for (unsigned i = 1; i < opts.size(); i++)
        {
            if (!myStr2Unsigned(opts[i], _miloa[i - 1]))
            {
                errMsg = "number of variables(p)!!";
                throw ILLEGAL_NUM;
            }
        }

        _gateList.resize(_miloa[0] + _miloa[3] + 1);

        // read in input
        if (_miloa[1] > 0)
        {
            for (size_t i = 0; i < _miloa[1]; i++)
            {
                f.getline(str, MAX_BUF_LEN, '\n');
                lineNo++;
                num = atoi(str);
                if(num %2 != 0)
                {
                    errInt = num;
                    throw CANNOT_INVERTED;
                }
                IsValidID(this, num);
                IsRDEF(this, num);
                // Create PI gate
                CirGate *g = new PI_gate(num);
                _gateList[num / 2] = g;
                g->setLineNo(lineNo);
                _input.push_back(g->_id);
    #ifdef DEBUG_MSG
                cerr << " IN[" << i << "]: " << g << endl;
    #endif
                _gateListSize++;
            }
        }

        // read in latch
        /*
        if (_miloa[2] > 0)
        {
            for (size_t i = 0; i < _miloa[2]; i++)
            {
                f.getline(str, MAX_BUF_LEN, '\n');
                lineNo++;
                num = atoi(str);
                CirGate *g = new LATCH_gate(num);
                _gateList.push_back(g);
            }
        }*/

        // read in output
        if (_miloa[3] > 0)
        {
            // get "M" to caculate PO's gate IDs
            unsigned n = _miloa[0];
            unsigned id = 0;
            for (unsigned i = 0; i < _miloa[3]; i++)
            {
                f.getline(str, MAX_BUF_LEN, '\n');
                lineNo++;
                num = atoi(str);
                // Create PO gate
                n++;
                id = n * 2;
                IsValidID(this, num);
                CirGate *g = new PO_gate(id, num);
                _gateList[n] = g;
                g->setLineNo(lineNo);
                _output.push_back(g->_id);

    #ifdef DEBUG_MSG
                cerr << "OUT[" << i << "]: " << g << endl;
    #endif
                _gateListSize++;
            }
        }

        // read in AIG gates
        if (_miloa[4] > 0)
        {
            unsigned t[3];
            // spilit
            for (unsigned i = 0; i < _miloa[4]; i++)
            {
                f.getline(str, MAX_BUF_LEN, '\n');
                lineNo++;
                ptr = strtok(str, " ");
                t[0] = atoi(ptr);
                ptr = strtok(NULL, " ");
                t[1] = atoi(ptr);
                ptr = strtok(NULL, " ");
                t[2] = atoi(ptr);
                if(t[0] % 2 != 0)
                {
                    errInt = t[0];
                    throw CANNOT_INVERTED;
                }
                IsValidID(this, t[0]);
                IsValidID(this, t[1]);
                IsValidID(this, t[2]);
                IsRDEF(this, t[0]);

                CirGate *g = new AIG_gate(t[0], t[1], t[2]);
                _gateList[t[0] / 2] = g;
                g->setLineNo(lineNo);
                _aig.push_back(g->_id);

    #ifdef DEBUG_MSG
                cerr << "AIG[" << i << "]: " << g << endl;
    #endif
                _gateListSize++;
            }
        }

        // Optional symbol or comment
        while (f.getline(str, MAX_BUF_LEN, '\n'))
        {
            char c = str[0]; // extract first character
            char *ptr;
            str[0] = ' ';
            switch (c)
            {
            case 'i':
                ptr = strtok(str, " ");
                num = atoi(ptr);

                ptr = strtok(NULL, " ");
                s_str.assign(ptr);
                // check redefined symbol
                if (_gateList[_input[num]]->_symbol != NULL) 
                {
                    errMsg = _gateList[_input[num]]->getTypeStr();
                    errInt = _gateList[_input[num]]->getID();
                    cout << _gateList[_input[num]]->_symbol;
                    throw REDEF_SYMBOLIC_NAME;
                }

                _gateList[_input[num]]->_symbol = new string(s_str);
    #ifdef DEBUG_MSG
                cerr << "IO.G: " << num << " , ";
                cerr << "IO.S: " << *(_gateList[_input[num]]->_symbol) << endl;
    #endif
                break;
            case 'o':
                ptr = strtok(str, " ");
                num = atoi(ptr);
                ptr = strtok(NULL, " ");
                s_str.assign(ptr);
                if (_gateList[_output[num]]->_symbol != NULL) 
                {
                    errMsg = _gateList[_output[num]]->getTypeStr();
                    errInt = _gateList[_output[num]]->getID();
                    cout << _gateList[_output[num]]->_symbol;
                    throw REDEF_SYMBOLIC_NAME;
                }
                _gateList[_output[num]]->_symbol = new string(s_str);
    #ifdef DEBUG_MSG
                cerr << "IO.G: " << num << " , ";
                cerr << "IO.S: " << *(_gateList[_output[num]]->_symbol) << endl;
    #endif
                break;
            case 'c':
                while (f.getline(str, MAX_BUF_LEN, '\n'))
                {
                    s_str.assign(str);
                    _comments.push_back(s_str);
                }

    #ifdef DEBUG_MSG
                cerr << endl
                    << "=====Comments=====" << endl;
                for (unsigned i = 0; i < _comments.size(); i++)
                    cerr << _comments[i] << endl;
    #endif
                break;
            default:
                errMsg = c;
                throw ILLEGAL_SYMBOL_TYPE;
                break;
            }
        }
        
        f.close();

        buildConnection();
        dfsTraversal(_output);
    }
    catch (CirParseError err)
    {
        read_status = false;
        parseError(err);
    }

    return read_status;
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
    cout << endl << "Circuit Statistics" << endl;
    cout << "==================" << endl;
    cout << "  PI";
    cout << setw(12) << right << _miloa[1] << endl;
    cout << "  PO";
    cout << setw(12) << right << _miloa[3] << endl;
    cout << "  AIG";
    cout << setw(11) << right << _miloa[4] << endl;
    cout << "------------------" << endl;
    cout << "  Total";
    cout << setw(9) << right << (_miloa[1] + _miloa[3] + _miloa[4]) << endl;
}

void CirMgr::printNetlist() const
{
#ifdef DEBUG_MSG
    cerr << "DFS.L.S: " << _dfsList.size() << endl;
#endif
    cout << endl;
    unsigned n = 0;
    for (unsigned i = 0; i < _dfsList.size(); i++)
    {
        if (_dfsList[i]->_typeID == UNDEF_GATE) continue;
        cout << "[" << n << "] ";
        n++;
        if (_dfsList[i]->_typeID == CONST_GATE || _dfsList[i]->_typeID == PI_GATE)
            _dfsList[i]->printGate();
        else
        {
            cout << setw(3) << left << _dfsList[i]->_typeStr << " " << _dfsList[i]->_id << " ";
            if (_dfsList[i]->_typeID == AIG_GATE)
            {
                AIG_gate *g = dynamic_cast<AIG_gate *>(_dfsList[i]);
                if (_gateList[g->getIn1()]->_typeID == UNDEF_GATE) cout << '*';
                if (g->_inv1) cout << '!';

                cout << g->getIn1() << ' ';
                if (_gateList[g->getIn2()]->_typeID == UNDEF_GATE) cout << '*';
                if (g->_inv2) cout << '!';
                cout << g->getIn2();
            }
            else if (_dfsList[i]->_typeID == PO_GATE)
            {
                PO_gate *g = dynamic_cast<PO_gate *>(_dfsList[i]);
                if (g->_inv1)
                {
                    if (_gateList[g->getIn()]->_typeID == UNDEF_GATE) cout << '*';
                    cout << '!';
                }
                cout << g->getIn();
            }
        }

        if (_dfsList[i]->_symbol != NULL) // exists comments
            cout << " (" << *(_dfsList[i]->_symbol) << ")";
        cout << endl;
    }
}

void CirMgr::printPIs() const
{
    cout << "PIs of the circuit:";
    for (unsigned i = 0; i < _miloa[1]; i++)
        cout << ' ' << _gateList[_input[i]]->getID();
    cout << endl;
}

void CirMgr::printPOs() const
{
    cout << "POs of the circuit:";
    for (unsigned i = 0; i < _miloa[3]; i++)
        cout << ' ' << _gateList[_output[i]]->getID();
    cout << endl;
}

void CirMgr::printFloatGates() const
{
    IdList unused;
    IdList undef;
    for (unsigned i = 1; i < _gateList.size(); i++)
    {
        if (_gateList[i] == NULL)  continue;
        if (_gateList[i]->_typeID == CONST_GATE) continue;
        if (_gateList[i]->_typeID != PO_GATE && _gateList[i]->_outList.empty())
        {
            unused.push_back(i);
            continue;
        }
        for (unsigned j = 0; j < _gateList[i]->_inList.size(); j++)
        {
            if (_gateList[i]->_inList[j]->_typeID == UNDEF_GATE)
            {
                undef.push_back(i);
                break;
            }
        }
        for (unsigned k = 0; k < _gateList[i]->_inList2.size(); k++)
        {
            if (_gateList[i]->_inList2[k]->_typeID == UNDEF_GATE)
            {
                undef.push_back(i);
                break;
            }
        }
    }
    if (!undef.empty())
    {
        cout << "Gates with floating fanin(s):";
        for (unsigned i = 0; i < undef.size(); i++)
            cout << ' ' << undef[i];
        cout << endl;
    }
    if (!unused.empty())
    {
        cout << "Gates defined but not used  :";
        for (unsigned i = 0; i < unused.size(); i++)
            cout << ' ' << unused[i];
        cout << endl;
    }
}

void CirMgr::writeAag(ostream &outfile) const
{
    vector<AIG_gate *> _dfsAIGl;
    for (unsigned i = 0; i < _dfsList.size(); i++)
    {
        // extract AIG gate index
        if (_dfsList[i]->_typeID == AIG_GATE)
            _dfsAIGl.push_back(dynamic_cast<AIG_gate *>(_dfsList[i]));
    }
    // write first line
    outfile << "aag " << _miloa[0] << " " << _miloa[1] << " " << _miloa[2] << " " << _miloa[3] << " " << _dfsAIGl.size() << endl;

    for (unsigned i = 0; i < _input.size(); i++)
        outfile << (_gateList[_input[i]]->_id) * 2 << endl;
    for (unsigned i = 0; i < _output.size(); i++)
    {
        unsigned in = dynamic_cast<PO_gate *>(_gateList[_output[i]])->getIn();
        in = (_gateList[_output[i]]->_inv1) ? in * 2 + 1 : in * 2;
        outfile << in << endl;
    }
    for (unsigned i = 0; i < _dfsAIGl.size(); i++)
    {
        unsigned in;
        outfile << (_dfsAIGl[i]->_id) * 2 << " ";
        in = _dfsAIGl[i]->getIn1();
        in = (_dfsAIGl[i]->_inv1) ? in * 2 + 1 : in * 2;
        outfile << in << " ";
        in = _dfsAIGl[i]->getIn2();
        in = (_dfsAIGl[i]->_inv2) ? in * 2 + 1 : in * 2;
        outfile << in << endl;
    }
    for (unsigned i = 0; i < _input.size(); i++)
    {
        if (_gateList[_input[i]]->_symbol != NULL)
            outfile << "i" << i << " " << *(_gateList[_input[i]]->_symbol) << endl;
    }
    for (unsigned i = 0; i < _output.size(); i++)
    {
        if (_gateList[_output[i]]->_symbol != NULL)
            outfile << "o" << i << " " << *(_gateList[_output[i]]->_symbol) << endl;
    }
    outfile << "c" << endl << AAG_OPT_COMMENT << endl;
}



CirGate *CirMgr::findGate(const unsigned &gid, const GateList &l) const
{
    for (unsigned i = 0; i < l.size(); i++)
    {
        //to avoid empty gates
        if (l[i] == NULL) continue;
        if (l[i]->getID() == gid)
            return l[i];
    }
    return NULL;
}

inline void CirMgr::IsValidID(CirMgr *Mgr, unsigned &gid)
{
    if (gid / 2 > _miloa[0])
    {
        errInt = gid / 2;
        throw MAX_LIT_ID;
    }
}

void CirMgr::IsRDEF(CirMgr *Mgr, unsigned &gid)
{
    if(_gateList[gid / 2] != NULL)
    {
        if(gid == 0) throw REDEF_CONST;
        if(_gateList[gid / 2]->_typeID != UNDEF_GATE)
        {
            errGate = _gateList[gid / 2];
            throw REDEF_GATE;
        }
    }
}

// Build connection between gates
void CirMgr::buildConnection()
{
    // Build from output first
    for (unsigned i = 0; i < _miloa[3]; i++)
    {
        unsigned in;
        in = dynamic_cast<PO_gate *>(_gateList[_output[i]])->getIn();
        // get output gate size
        CirGate *g = _gateList[in];
        if (g != NULL)
            _gateList[_output[i]]->addFin(g);
        else
        {
            CirGate *u = new UNDEF_gate(in);
            _gateList[in] = u;
            _gateList[_output[i]]->addFin(u);
        }
    }
    // AIG gate
    for (unsigned i = 0; i < _miloa[4]; i++)
    {
        unsigned in;
        in = dynamic_cast<AIG_gate *>(_gateList[_aig[i]])->getIn1();
        CirGate *g = _gateList[in];
        if (g != NULL)
            _gateList[_aig[i]]->addFin(g);
        else
        {
            CirGate *u = new UNDEF_gate(in);
            _gateList[in] = u;
            _gateList[_aig[i]]->addFin(u);
        }

        in = dynamic_cast<AIG_gate *>(_gateList[_aig[i]])->getIn2();
        g = _gateList[in];
        if (g != NULL)
            _gateList[_aig[i]]->addFin2(g);
        else
        {
            CirGate *u = new UNDEF_gate(in);
            _gateList[in] = u;
            _gateList[_aig[i]]->addFin2(u);
        }
    }
    // DO NOT NEED to connect PI gates, it has been finished by above operations.
}

void CirMgr::dfsTraversal(const IdList &sinkList)
{
    CirGate::setGlobalRef();
    for (auto &i : sinkList)
        _gateList[i]->dfsTraversal(_gateList[i], _dfsList);
}