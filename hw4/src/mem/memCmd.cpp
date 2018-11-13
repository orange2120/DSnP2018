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
    // Check options at prefix position
    if (myStrNCmp("-Index", options[0], 2) == 0 || myStrNCmp("-Random", options[0], 2) == 0 || myStrNCmp("-Array", options[0], 2) == 0)
    {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    }
    // Check current mode
    for (size_t i = 0; i < options.size(); i++)
    {
        if (myStrNCmp("-Array", options[i], 2) == 0)
        {
            if (arrayMode)
            {
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[i]);
            }
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
        if (arr_size_idx == 1)
        {
            if (!myStr2Int(options[arr_size_idx + 1], obj_size) || obj_size <= 0)
            {
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[arr_size_idx - 2]);
            }
        }
        else
        {
            if (!myStr2Int(options[arr_size_idx - 2], obj_size) || obj_size <= 0)
            {
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[arr_size_idx - 2]);
            }
        }

        try
        {
            mtest.newArrs(obj_size, arr_size);
        }
        catch (bad_alloc)
        {
            return CMD_EXEC_ERROR;
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
            catch (bad_alloc)
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
    int idx_opt_idx = 0; // "-Index" option index
    int rnd_opt_idx = 0; // "-Random" option index

    int validCmdPfxIdx = 0;

    bool idxMode = false;
    bool rnMode = false;
    bool arrayMode = false;

    // Valid command in prefix position
    // "-i","-a","-r" + any
    bool repeatOpt = false;

    // Valid command in position position
    // "-r","-i" <size_t>, + "-i","-r" -> Extra
    //                     + others    -> Illegal

    // TODO

    for (size_t i = 0; i < options.size(); i++)
    {
        if (myStrNCmp("-Index", options[i], 2) == 0)
        {
            if (idxMode)
                repeatOpt = true;
            idxMode = true; // Found "-i"
            idx_opt_idx = i;
        }
        else if (myStrNCmp("-Random", options[i], 2) == 0)
        {
            if (idxMode)
                repeatOpt = true;
            rnMode = true; // Found "-r"
            rnd_opt_idx = i;
        }
        if (myStrNCmp("-Array", options[i], 2) == 0)
        {
            if (idxMode)
                repeatOpt = true;
            arrayMode = true; // Found "-a"
        }
    }
    // Handle "-Array" option
    if (options.size() == 1 && arrayMode)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    else if (options.size() == 1)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    // Check options at prefix position == 0
    if (!(myStrNCmp("-Index", options[0], 2) == 0) && !(myStrNCmp("-Random", options[0], 2) == 0) && !(myStrNCmp("-Array", options[0], 2) == 0))
    {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    }

    if (!arrayMode && options.size() > 2)
    {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.back());
    }

    // Special case: mtd -r 23 -r, mtd -r 23 -i <- Extra, others -> illegal >> idx,rn both true

    // Get parameters after "-Index" and "-Random"
    // 1. there exists parameters
    // 2. the paramter is valid
    if (!idxMode && !rnMode)
    {
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.front());
    }
    else
    {
        // *** THERE MUST EXIST PARAMETER ***
        // using boundary checking
        // if option command is at the back, it MUST BE paramter less
        if (options.size() - 1 == (size_t)idx_opt_idx || options.size() - 1 == (size_t)rnd_opt_idx)
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options.front());
        }
        if (idxMode)
        {
            // Check "-i" exists parameter
            if (options.size() - 1 == (size_t)idx_opt_idx)
                return CmdExec::errorOption(CMD_OPT_MISSING, "");
            if (!myStr2Int(options[idx_opt_idx + 1], objId) || objId < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_opt_idx + 1]);
            if (objId > 0)
            {
                validCmdPfxIdx = idx_opt_idx + 1;
            }
            else
                validCmdPfxIdx = idx_opt_idx;
        }
        else if (rnMode)
        {
            // Check "-r" exists parameter
            if (options.size() - 1 == (size_t)rnd_opt_idx)
                return CmdExec::errorOption(CMD_OPT_MISSING, "");
            if (!myStr2Int(options[rnd_opt_idx + 1], numRandId) || numRandId < 0)
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[rnd_opt_idx + 1]);
            if (numRandId > 0)
            {
                validCmdPfxIdx = rnd_opt_idx + 1;
            }
            else
                validCmdPfxIdx = rnd_opt_idx;
        }
    }
    if (options.size() - 1 > (size_t)rnd_opt_idx)

        if (options.size() - 2 > (size_t)validCmdPfxIdx)
        {
            validCmdPfxIdx++;
            if (((myStrNCmp("-Index", options[validCmdPfxIdx], 2) == 0) || (myStrNCmp("-Random", options[validCmdPfxIdx], 2) == 0) || (myStrNCmp("-Array", options[validCmdPfxIdx], 2) == 0)))
            {
                return CmdExec::errorOption(CMD_OPT_EXTRA, options[validCmdPfxIdx]);
            }
            else
            {
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[validCmdPfxIdx]);
            }
        }
    // Found "-i"
    if (idxMode)
    {
        if (arrayMode)
        {
            /*if (mtest.getArrListSize() == 0)
            {
                cerr << "Size of array list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_opt_idx]);
            }*/
            if (mtest.getArrListSize() <= (size_t)objId)
            {
                cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_opt_idx + 1]);
            }
            mtest.deleteArr(objId);
        }
        else
        {
            if (mtest.getObjListSize() <= (size_t)objId)
            {
                cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!" << endl;
                return CMD_EXEC_ERROR;
            }
            mtest.deleteObj(objId);
        }
    }
    else if (rnMode)
    {
        if (mtest.getArrListSize() == 0 && arrayMode)
        {
            cerr << "Size of array list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[rnd_opt_idx]);
        }
        /*else if (mtest.getObjListSize() == 0 && !arr)
        {
            cerr << "Size of object list is 0!!" << endl;
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[rnd_opt_idx]);
        }*/

        for (int i = 0; i < numRandId; i++)
        {
            if (arrayMode)
            {
                if (mtest.getArrListSize() > 0)
                    mtest.deleteArr(rnGen(mtest.getArrListSize()));
            }
            else
            {
                if (mtest.getObjListSize() > 0)
                    mtest.deleteObj(rnGen(mtest.getObjListSize()));
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
