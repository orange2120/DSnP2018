/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest; // defined in memTest.cpp

bool initMemCmd()
{
    if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
          cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
          cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
          cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)))
    {
        cerr << "Registering \"mem\" commands fails... exiting" << endl;
        return false;
    }
    return true;
}

//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string &option)
{
    // check option
    string token;
    if (!CmdExec::lexSingleOption(option, token))
        return CMD_EXEC_ERROR;
    if (token.size())
    {
        int b;
        if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj))))
        {
            cerr << "Illegal block size (" << token << ")!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
        }
#ifdef MEM_MGR_H
        mtest.reset(toSizeT(b));
#else
        mtest.reset();
#endif // MEM_MGR_H
    }
    else
        mtest.reset();
    return CMD_EXEC_DONE;
}

void MTResetCmd::usage(ostream &os) const
{
    os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void MTResetCmd::help() const
{
    cout << setw(15) << left << "MTReset: "
         << "(memory test) reset memory manager" << endl;
}

//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string &option)
{
    // TODO
    vector<string> options;
    if (!lexOptions(option, options))
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    if (options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    else if (options.size() > 3)
    {
        return CmdExec::errorOption(CMD_OPT_EXTRA, options.back());
    }

    int arr_size = 0;
    int obj_size = 0;
    int arr_size_idx = 0; // Options array size index
    bool arrayMode = false;

    // Check current mode
    for (size_t i = 0; i < options.size(); i++)
    {
        if (myStrNCmp("-Array", options[i], 1) == 0)
        {
            arrayMode = true;
            arr_size_idx = i + 1;
            break;
        }
    }

    if (arrayMode)
    {
        if (options.size() < 3)
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
        // Return illegal array size
        if (!myStr2Int(options[arr_size_idx], arr_size) || arr_size <= 0)
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.back());
        }
        // Return illegal object size
        if (!myStr2Int(options[arr_size_idx - 2], obj_size) || obj_size <= 0)
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[arr_size_idx - 2]);
        }
        else
        {
            try
            {
                mtest.newArrs(obj_size, arr_size);
            }
            catch (bad_alloc())
            {
                return CMD_EXEC_ERROR;
            }
        }
    }
    else
    {
        if (options.size() > 1)
        {
            return CmdExec::errorOption(CMD_OPT_EXTRA, options.back());
        }
        if (!myStr2Int(options.back(), obj_size) || obj_size <= 0)
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.back());
        }
        else
        {
            try
            {
                mtest.newObjs(obj_size);
            }
            catch (bad_alloc())
            {
                return CMD_EXEC_ERROR;
            }
        }
    }
    // Use try-catch to catch the bad_alloc exception
    return CMD_EXEC_DONE;
}

void MTNewCmd::usage(ostream &os) const
{
    os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void MTNewCmd::help() const
{
    cout << setw(15) << left << "MTNew: "
         << "(memory test) new objects" << endl;
}

//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string &option)
{
    // TODO
    vector<string> options;
    if (!lexOptions(option, options))
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    if (options.empty())
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    // To record which mode to operation

    int objId = 0;
    int numRandId = 0;
    int tmp_num = 0;
    bool idxMode = false;
    bool rnMode = false;
    bool arrayMode = false;

    for (size_t i = 0; i < options.size(); i++)
    {
        // TODO 尋找 opt後面的東西，並判斷是文字還是數字
        if (myStrNCmp("-Index", options[i], 1) == 0)
        {
            idxMode = true;
            if (!myStr2Int(options[i + 1], tmp_num))
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
            objId = (size_t)tmp_num;
        }
        else if (myStrNCmp("-Random", options[i], 1) == 0)
        {
            rnMode = true;
            if (!myStr2Int(options[i + 1], tmp_num))
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i + 1]);
            numRandId = (size_t)tmp_num;
        }
        if (myStrNCmp("-Array", options[i], 1) == 0)
            arrayMode = true;
    }

    if (!arrayMode && options.size() > 2)
    {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.back());
    }

    if (idxMode)
    {
        if (arrayMode)
        {
            mtest.deleteArr(objId);
        }
        else
        {
            mtest.deleteObj(objId);
        }
    }
    else if (rnMode)
    {
        rnGen(0);
        for (size_t i = 0; i < numRandId; i++)
        {
            if (arrayMode)
            {
                mtest.deleteArr(rnGen(numRandId));
            }
            else
            {
                mtest.deleteObj(rnGen(numRandId));
            }
        }
    }
    else
    {
        return CMD_EXEC_ERROR;
    }

    return CMD_EXEC_DONE;
}

void MTDeleteCmd::usage(ostream &os) const
{
    os << "Usage: MTDelete <-Index (size_t objId) | "
       << "-Random (size_t numRandId)> [-Array]" << endl;
}

void MTDeleteCmd::help() const
{
    cout << setw(15) << left << "MTDelete: "
         << "(memory test) delete objects" << endl;
}

//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string &option)
{
    // check option
    if (option.size())
        return CmdExec::errorOption(CMD_OPT_EXTRA, option);
    mtest.print();

    return CMD_EXEC_DONE;
}

void MTPrintCmd::usage(ostream &os) const
{
    os << "Usage: MTPrint" << endl;
}

void MTPrintCmd::help() const
{
    cout << setw(15) << left << "MTPrint: "
         << "(memory test) print memory manager info" << endl;
}
