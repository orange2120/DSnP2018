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

    size_t nOpts = options.size();

    int arr_size = 0;
    int obj_size = 0;
    int arr_opt_idx = 0; // Options array size index

    bool arrayMode = false;

    //  Must be 1 < nOpts < 3
    if (nOpts == 0)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    else if (nOpts > 3)
        return CmdExec::errorOption(CMD_OPT_EXTRA, options[3]);

    // Check mode
    for (size_t i = 0; i < nOpts; i++)
    {
        if (myStrNCmp("-Array", options[i], 2) == 0)
        {
            arrayMode = true;
            arr_opt_idx = i;
            break;
        }
    }

    // "-Array" is at prefix position -> mtn -Array <arr_size> <obj_size
    if (arrayMode && arr_opt_idx == 0)
    {
        if (nOpts < 3)
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
        // Reject illegal array size
        if (!myStr2Int(options[1], arr_size) || arr_size <= 0)
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
        }
        if (!myStr2Int(options[2], obj_size) || obj_size <= 0)
        {
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
        }
    }

    // "-Array" is at other position, but absence of parameter -> mtn <obj_size> -Array
    else if (arrayMode && arr_opt_idx == 1 && nOpts == 2)
        return CmdExec::errorOption(CMD_OPT_MISSING, options[1]);

    // "-Array" is at other position -> mtn <obj_size> -Array <arr_size>
    else if (arrayMode)
    {
        if (!myStr2Int(options[0], obj_size) || obj_size <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
        if (!myStr2Int(options[2], arr_size) || arr_size <= 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
    }
    // Non-array mode
    else
    {
        if (!myStr2Int(options[0], obj_size))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
        if (nOpts > 1)
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[1]);
        if (!myStr2Int(options[0], obj_size) || obj_size < 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
    }

    if (arrayMode)
    {
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
        try
        {
            mtest.newObjs(obj_size);
        }
        catch (bad_alloc)
        {
            return CMD_EXEC_ERROR;
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

    size_t nOpts = options.size();

    // To record which mode to operation
    int objId = 0;
    int numRandId = 0;
    int tmp = 0;
    int idx_rnd_opt_idx = 0; // "-Index" or "-Random" option index
    int arr_opt_idx = 0;
    int repeat_idx = 0;

    bool idxMode = false;
    bool rnMode = false;
    bool arrayMode = false;
    bool repeats = false;
    bool repeats_arr = false;

    if (nOpts == 0)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");

    // Go through options
    for (size_t i = 0; i < nOpts; i++)
    {
        if (myStrNCmp("-Index", options[i], 2) == 0)
        {
            if (idxMode || rnMode)
            {
                repeat_idx = i;
                repeats = true;
            }
            else
            {
                idxMode = true; // Found "-Index"
                idx_rnd_opt_idx = i;
            }
        }
        else if (myStrNCmp("-Random", options[i], 2) == 0)
        {
            if (idxMode || rnMode)
            {
                repeat_idx = i;
                repeats = true;
            }
            else
            {
                rnMode = true; // Found "-Random"
                idx_rnd_opt_idx = i;
            }
        }
        if (myStrNCmp("-Array", options[i], 2) == 0)
        {
            if (arrayMode)
            {
                repeat_idx = i;
                repeats_arr = true;
            }
            else if (i < 4)
            {
                arrayMode = true; // Found "-Array"
                arr_opt_idx = i;
            }
        }
    }

    // Handle repeat
    // First option MUST BE "-Array" or "-Index" or "-Random"
    if (myStrNCmp("-Index", options[0], 2) && myStrNCmp("-Random", options[0], 2) && myStrNCmp("-Array", options[0], 2))
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    if (!arrayMode && !rnMode && !idxMode)
        return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[0]);
    // Missing arguments
    if (nOpts == 1 && arrayMode)
        return CmdExec::errorOption(CMD_OPT_MISSING, "");
    else if (nOpts == 1)
        return CmdExec::errorOption(CMD_OPT_MISSING, options[0]);

    // nOpts >= 2
    // Handle "-Index" or "-Random" at prefix position
    if (arrayMode && arr_opt_idx == 0)
    {
        if (!rnMode && !idxMode)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[1]);
    }

    if (rnMode || idxMode)
    {
        // Handle extra options
        if (arrayMode && nOpts > 3 && arr_opt_idx != 0)
        {
            if (myStrNCmp("-Random", options[arr_opt_idx + 1], 2) == 0 || myStrNCmp("-Index", options[arr_opt_idx + 1], 2) == 0 || myStrNCmp("-Array", options[arr_opt_idx + 1], 2) == 0)
                return CmdExec::errorOption(CMD_OPT_EXTRA, options[arr_opt_idx + 1]);
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[arr_opt_idx + 1]);
        }

        if (nOpts > 3 && repeats_arr)
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[arr_opt_idx + 1]);
        else if (nOpts > 3 && !repeats && arrayMode)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[arr_opt_idx + 1]);
        else if (nOpts > 3 && !arrayMode && (repeats || repeats_arr))
            return CmdExec::errorOption(CMD_OPT_EXTRA, options[repeat_idx]);

        if (!myStr2Int(options[idx_rnd_opt_idx + 1], tmp) || tmp < 0)
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_rnd_opt_idx + 1]);

        // There are "-Array" or other options
        if (nOpts > 2 && !arrayMode)
        {
            if (repeat_idx > 0 && repeats)

                return CmdExec::errorOption(CMD_OPT_EXTRA, options[repeat_idx]);
            else
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[2]);
        }
        // Array mode

        if (rnMode)
            numRandId = tmp;
        else
            objId = tmp;
    }

    // Special case: mtd -r 23 -r, mtd -r 23 -i <- Extra, others -> illegal >> idx,rn both true

    // Get parameters after "-Index" and "-Random"
    // 1. there exists parameters
    // 2. the paramter is valid
    // *** THERE MUST EXIST PARAMETER ***
    // using boundary checking
    // if option command is at the back, it MUST BE paramter less

    if (idxMode)
    {
        if (arrayMode)
        {
            if (mtest.getArrListSize() <= (size_t)objId)
            {
                cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << objId << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_rnd_opt_idx + 1]);
            }
            mtest.deleteArr(objId);
        }
        else
        {
            if (mtest.getObjListSize() <= (size_t)objId)
            {
                cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << objId << "!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_rnd_opt_idx + 1]);
            }
            mtest.deleteObj(objId);
        }
    }
    else if (rnMode)
    {
        if (arrayMode)
        {
            if (mtest.getArrListSize() == 0)
            {
                cerr << "Size of array list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_rnd_opt_idx]);
            }
            else
            {
                for (int i = 0; i < numRandId; i++)
                {
                    if (mtest.getArrListSize() > 0)
                        mtest.deleteArr(rnGen(mtest.getArrListSize()));
                }
            }
        }
        else
        {
            if (mtest.getObjListSize() == 0)
            {
                cerr << "Size of object list is 0!!" << endl;
                return CmdExec::errorOption(CMD_OPT_ILLEGAL, options[idx_rnd_opt_idx]);
            }
            else
            {
                for (int i = 0; i < numRandId; i++)
                {
                    if (mtest.getObjListSize() > 0)
                        mtest.deleteObj(rnGen(mtest.getObjListSize()));
                }
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
