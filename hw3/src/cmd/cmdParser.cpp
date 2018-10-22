/****************************************************************************
  FileName     [ cmdParser.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command parsing member functions for class CmdParser ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include "util.h"
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    External funcitons
//----------------------------------------------------------------------
void mybeep();

//----------------------------------------------------------------------
//    Member Function for class cmdParser
//----------------------------------------------------------------------
// return false if file cannot be opened
// Please refer to the comments in "DofileCmd::exec", cmdCommon.cpp
bool CmdParser::openDofile(const string &dof)
{
    _dofile = new ifstream(dof.c_str());
    if (!_dofile->is_open())
    {
        // Clear the pointer
        if (_dofileStack.size() > 0)
            _dofile = _dofileStack.top();
        else
            _dofile = 0;
        return false;
    }

    if (_dofileStack.size() <= 1024)
        _dofileStack.push(_dofile);
    else
        return false;

    return true;
}

// Must make sure _dofile != 0
void CmdParser::closeDofile()
{
    assert(_dofile != 0);

    _dofile->close();
    if (!_dofile->is_open())
    {
        delete _dofile;
        _dofileStack.pop();
        if (_dofileStack.size() > 0)
            _dofile = _dofileStack.top();
        else
            _dofile = 0;
    }
}

// Return false if registration fails
bool CmdParser::regCmd(const string &cmd, unsigned nCmp, CmdExec *e)
{
    // Make sure cmd hasn't been registered and won't cause ambiguity
    string str = cmd; // Make a duplicate
    unsigned s = str.size();
    if (s < nCmp) // If condition not sufficient, return false
        return false;
    while (true)
    {
        if (getCmd(str))
            return false;
        if (s == nCmp)
            break;
        str.resize(--s);
    }

    // Change the first nCmp characters to upper case to facilitate
    //    case-insensitive comparison later.
    // The strings stored in _cmdMap are all upper case
    //
    assert(str.size() == nCmp); // str is now mandCmd
    string &mandCmd = str;
    for (unsigned i = 0; i < nCmp; ++i)
        mandCmd[i] = toupper(mandCmd[i]);
    string optCmd = cmd.substr(nCmp);
    assert(e != 0);
    e->setOptCmd(optCmd);

    // insert (mandCmd, e) to _cmdMap; return false if insertion fails.
    return (_cmdMap.insert(CmdRegPair(mandCmd, e))).second;
}

// Return false on "quit" or if excetion happens
CmdExecStatus
CmdParser::execOneCmd()
{
    bool newCmd = false;
    if (_dofile != 0)
        newCmd = readCmd(*_dofile);
    else
        newCmd = readCmd(cin);

    // execute the command
    if (newCmd)
    {
        string option;
        CmdExec *e = parseCmd(option);
        if (e != 0)
            return e->exec(option);
    }
    return CMD_EXEC_NOP;
}

// For each CmdExec* in _cmdMap, call its "help()" to print out the help msg.
// Print an endl at the end.
void CmdParser::printHelps() const
{
    map<const string, CmdExec *>::iterator it;

    for (auto it = _cmdMap.begin(); it != _cmdMap.end(); it++)
    {
        it->second->help();
    }
    cout << endl;
}

void CmdParser::printHistory(int nPrint) const
{
    assert(_tempCmdStored == false);
    if (_history.empty())
    {
        cout << "Empty command history!!" << endl;
        return;
    }
    int s = _history.size();
    if ((nPrint < 0) || (nPrint > s))
        nPrint = s;
    for (int i = s - nPrint; i < s; ++i)
        cout << "   " << i << ": " << _history[i] << endl;
}

//
// Parse the command from _history.back();
// Let string str = _history.back();
//
// 1. Read the command string (may contain multiple words) from the leading
//    part of str (i.e. the first word) and retrive the corresponding
//    CmdExec* from _cmdMap
//    ==> If command not found, print to cerr the following message:
//        Illegal command!! "(string cmdName)"
//    ==> return it at the end.
// 2. Call getCmd(cmd) to retrieve command from _cmdMap.
//    "cmd" is the first word of "str".
// 3. Get the command options from the trailing part of str (i.e. second
//    words and beyond) and store them in "option"
//
CmdExec *
CmdParser::parseCmd(string &option)
{
    assert(_tempCmdStored == false);
    assert(!_history.empty());
    string str = _history.back();
    string cmd;
    size_t opt_offset = 0;

    opt_offset = myStrGetTok(str, cmd);

    CmdExec *e = getCmd(cmd);

    if (str.size() > cmd.size())
        option = str.substr(opt_offset, str.size());

    if (e != 0)
        return e;

    cerr << "Illegal command!! (" << cmd << ")" << endl;

    assert(str[0] != 0 && str[0] != ' ');
    return NULL;
}

// Remove this function for TODO...
//
// This function is called by pressing 'Tab'.
// It is to list the partially matched commands.
// "str" is the partial string before current cursor position. It can be
// a null string, or begin with ' '. The beginning ' ' will be ignored.
//
// Several possibilities after pressing 'Tab'
// (Let $ be the cursor position)
// 1. LIST ALL COMMANDS
//    --- 1.1 ---
//    [Before] Null cmd
//    cmd> $
//    --- 1.2 ---
//    [Before] Cmd with ' ' only
//    cmd>     $
//    [After Tab]
//    ==> List all the commands, each command is printed out by:
//           cout << setw(12) << left << cmd;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location (including ' ')
//
// 2. LIST ALL PARTIALLY MATCHED COMMANDS
//    --- 2.1 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$                   // partially matched
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$                   // and then re-print the partial command
//    --- 2.2 ---
//    [Before] partially matched (multiple matches)
//    cmd> h$llo                // partially matched with trailing characters
//    [After Tab]
//    HELp        HIStory       // List all the parially matched commands
//    cmd> h$llo                // and then re-print the partial command
//
// 3. LIST THE SINGLY MATCHED COMMAND
//    ==> In either of the following cases, print out cmd + ' '
//    ==> and reset _tabPressCount to 0
//    --- 3.1 ---
//    [Before] partially matched (single match)
//    cmd> he$
//    [After Tab]
//    cmd> heLp $               //
//    --- 3.2 ---
//    [Before] partially matched with trailing characters (single match)
//    cmd> he$ahah
//    [After Tab]
//    cmd> heLp $ahaha
//    ==> Automatically complete on the same line
//    ==> The auto-expanded part follow the strings stored in cmd map and
//        cmd->_optCmd. Insert a space after "heLp"
//    --- 3.3 ---
//    [Before] fully matched (cursor right behind cmd)
//    cmd> hElP$sdf
//    [After Tab]
//    cmd> hElP $sdf            // a space character is inserted
//
// 4. NO MATCH IN FITST WORD
//    --- 4.1 ---
//    [Before] No match
//    cmd> hek$
//    [After Tab]
//    ==> Beep and stay in the same location
//
// 5. FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
//    --- 5.1 ---
//    [Before] Already matched on first tab pressing
//    cmd> help asd$gh
//    [After] Print out the usage for the already matched command
//    Usage: HELp [(string cmd)]
//    cmd> help asd$gh
//
// 6. FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
//    ==> Note: command usage has been printed under first tab press
//    ==> Check the word the cursor is at; get the prefix before the cursor
//    ==> So, this is to list the file names under current directory that
//        match the prefix
//    ==> List all the matched file names alphabetically by:
//           cout << setw(16) << left << fileName;
//    ==> Print a new line for every 5 commands
//    ==> After printing, re-print the prompt and place the cursor back to
//        original location
//    --- 6.1 ---
//    Considering the following cases in which prefix is empty:
//    --- 6.1.1 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and they do not have a common prefix,
//    cmd> help $sdfgh
//    [After] print all the file names
//    .               ..              Homework_3.docx Homework_3.pdf  Makefile
//    MustExist.txt   MustRemove.txt  bin             dofiles         include
//    lib             mydb            ref             src             testdb
//    cmd> help $sdfgh
//    --- 6.1.2 ---
//    [Before] if prefix is empty, and in this directory there are multiple
//             files and all of them have a common prefix,
//    cmd> help $orld
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help mydb-$orld
//    --- 6.1.3 ---
//    [Before] if prefix is empty, and only one file in the current directory
//    cmd> help $ydb
//    [After] print out the single file name followed by a ' '
//    cmd> help mydb $
//    --- 6.2 ---
//    [Before] with a prefix and with mutiple matched files
//    cmd> help M$Donald
//    [After]
//    Makefile        MustExist.txt   MustRemove.txt
//    cmd> help M$Donald
//    --- 6.3 ---
//    [Before] with a prefix and with mutiple matched files,
//             and these matched files have a common prefix
//    cmd> help Mu$k
//    [After]
//    ==> auto insert the common prefix and make a beep sound
//    ==> DO NOT print the matched files
//    cmd> help Must$k
//    --- 6.4 ---
//    [Before] with a prefix and with a singly matched file
//    cmd> help MustE$aa
//    [After] insert the remaining of the matched file name followed by a ' '
//    cmd> help MustExist.txt $aa
//    --- 6.5 ---
//    [Before] with a prefix and NO matched file
//    cmd> help Ye$kk
//    [After] beep and stay in the same location
//    cmd> help Ye$kk
//
//    [Note] The counting of tab press is reset after "newline" is entered.
//
// 7. FIRST WORD NO MATCH
//    --- 7.1 ---
//    [Before] Cursor NOT on the first word and NOT matched command
//    cmd> he haha$kk
//    [After Tab]
//    ==> Beep and stay in the same location

void CmdParser::listCmd(const string &str)
{
    // TODO:...
    // 任何補齊指令都要把後面的字串放回去
    // 無論如何 先切割
    // 有換行 tab就要設定成0
    // 先檢查指令是否有效 無效->mybeep
    // 有效的話要看指標指在cmd 還是 filename上面
    // 如果指令未完全 且 後部有fileprefix -> print dir
    // 先 如果tab = 1 print usage
    // tab = 2 再 print filename

    // { tab 1 次

    //}
    // 如果指令未完全 且 後部無 -> 補齊指令 + " "
    ///
    //
    // Erase blank part
    string trimed_cmd = str;
    trimed_cmd.erase(0, trimed_cmd.find_first_not_of(" "));
    trimed_cmd.erase(trimed_cmd.find_last_not_of(" ") + 1);

    //cerr << trimed_cmd << "," << effect_cmd << endl;

    if (_tabPressCount >= 1)
    {
        // judge the command is valid or not

        // if the prefix string is empty, LIST ALL COMMANDS
        if (trimed_cmd.empty())
        {
            map<const string, CmdExec *>::iterator it;
            cout << endl;
            int counter = 0;
            for (it = _cmdMap.begin(); it != _cmdMap.end(); it++)
            {
                cout << setw(12) << left << it->first + it->second->getOptCmd();
                counter++;
                if (counter == 5)
                {
                    cout << endl;
                    counter = 0;
                }
            }
            cout << endl;
            resetBufAndPrintPrompt();
            _tabPressCount = 0;
            return;
        }
        else
        {
            printCmds(trimed_cmd);
        }
    }
}

// Print all commands or partial commands according to inptu string
void CmdParser::printCmds(const string &str)
{
    string cmd_name = "";   // complete commands to print
    string cmd_to_cmp = ""; // input commands to compare with complete commands

    string effect_cmd = str.substr(0, str.find(" "));
    map<const string, CmdExec *>::iterator it;
    vector<string> cmd_to_prt;

    // TODO:
    // 檢查cursor是在cmd上(後)面還是在filename上面 直接叫printfile或是cmd

    CmdExec *valid_cmd = 0;

    // LIST ALL PARTIALLY MATCHED COMMANDS
    for (it = _cmdMap.begin(); it != _cmdMap.end(); it++)
    {
        cmd_name = it->first + it->second->getOptCmd();
        cmd_to_cmp = cmd_name.substr(0, str.size());

        if (myStrNCmp(effect_cmd, cmd_to_cmp, effect_cmd.size()) == 0)
        {
            cmd_to_prt.push_back(cmd_name);
            valid_cmd = it->second;
        }
    }

    // NO MATCH IN FITST WORD
    if (cmd_to_prt.empty())
    {
        mybeep();
        _tabPressCount = 0;
        return;
    }

    // LIST THE SINGLY MATCHED COMMAND
    else if (cmd_to_prt.size() == 1)
    {
        // TODO: 判斷tab按下幾次
        // 若cmd後面 有接東西
        if (_tabPressCount > 2)
        {

            // filename example: test1.json test te...
            // dir_path example: ../tests
            // need to extract file_pfx and dir_path from full_path
            string full_path;
            string file_pfx;
            string dir_path;
            // if prefix is empty, example: dbprint $
            //                                    ^ blank
            if (*(_readBufPtr - 1) == ' ')
            {
                file_pfx = "";
                printDir(file_pfx, ".");
                return;
            }
            // prefix is not empty
            else
            {
                // get file name prefix from input string (find blank from string tail)
                full_path = str.substr(str.rfind(" "), str.size());
                full_path.erase(0, full_path.find_first_not_of(" "));
                full_path.erase(full_path.find_last_not_of(" ") + 1);

                // first find file_pfx
                if (full_path.rfind("/") != string::npos)
                {
                    file_pfx = full_path.substr(full_path.rfind("/") + 1, full_path.size());
                    dir_path = full_path.substr(0, full_path.rfind("/"));
                }
                // full path not include dir path (only file name prefix)
                else
                {
                    file_pfx = full_path;
                    dir_path = ".";
                }

                printDir(file_pfx, dir_path);
                return;
            }
        }
        // FIRST WORD ALREADY MATCHED ON FIRST TAB PRESSING
        // print usage
        else if (_tabPressCount == 2)
        {
            storePostStr();
            cout << endl;
            valid_cmd->usage(cout);
            //cout << endl;
            rePrintPostStr();
            reprintCmd();
            return;
        }
        // auto completed with a space inserted
        else if (_tabPressCount == 1)
        {
            storePostStr();
            for (size_t i = str.size(); i < (cmd_to_prt[0].size()); i++)
            {
                insertChar(cmd_to_prt[0][i]);
            }
            // insert a blank
            rePrintPostStr();
            insertChar(' ');
            return;
        }
        else
        {
            _tabPressCount = 0;
        }
        return;
    }

    // LIST ALL PARTIALLY MATCHED COMMANDS
    else
    {
        cout << endl;
        int counter = 0;
        for (size_t i = 0; i < cmd_to_prt.size(); i++)
        {
            cout << setw(12) << left << cmd_to_prt[i];
            counter++;
            if (counter == 5)
            {
                cout << endl;
                counter = 0;
            }
        }
        reprintCmd();
        _tabPressCount = 0;
        return;
    }
}

void CmdParser::printDir(const string &file_pfx, const string &path)
{
    // TODO:
    // 沒pfx print all the filenames
    // 有pfx 若唯一->補齊 不唯一->顯示所有符合條件
    // tab = 1 補齊
    // tab = 2 又 不唯一 顯示符合條件
    vector<string> file_name;

    // open dir error
    if (listDir(file_name, file_pfx, path) != 0)
    {
        mybeep();
        return;
    }

    // found one matching file name , first insert
    if (file_name.size() == 1)
    {
        // fill the whole file name from filename prefix
        // NOTE: here the file name is case sensitive
        moveBufPtr(_readBufPtr);
        for (size_t i = file_pfx.size(); i < (file_name[0].size()); i++)
        {
            insertChar(file_name[0][i]);
        }
    }

    // with a prefix and with mutiple matched files,
    // and these matched files have a common prefix
    else if (file_name.size() > 1 && !file_pfx.empty())
    {
        if (_tabPressCount >= 2)
        {
            // find the max common file name length
            size_t max_common_len = file_name[0].size();
            string common_fn = file_name[0];

            for (size_t i = 0; i < file_name.size() - 1; i++)
            {
                if ((strspn(file_name[i].c_str(), file_name[i + 1].c_str()) - 1) < max_common_len)
                {
                    max_common_len = strspn(file_name[i].c_str(), file_name[i + 1].c_str()) - 1;
                }
            }

            // place the cursor back to original location
            moveBufPtr(_readBufPtr);
            for (size_t k = file_pfx.size(); k < max_common_len; k++)
            {
                insertChar(common_fn[k]);
            }
        }

        // FIRST WORD ALREADY MATCHED ON SECOND AND LATER TAB PRESSING
    }

    else if (file_name.size() == 0)
    {
        cout << endl;
        int counter = 0;
        for (size_t i = 0; i < file_name.size(); i++)
        {
            cout << setw(16) << left << file_name[i];
            counter++;
            if (counter == 5)
            {
                cout << endl;
                counter = 0;
            }
        }
        reprintCmd();
    }
}

// store string after current _readBufPtr
void CmdParser::storePostStr(void)
{
    char *tmp_ptr = _readBufPtr;
    int i = 0;
    while (tmp_ptr <= _readBufEnd)
    {
        _back_cmd[i] += *tmp_ptr;
        tmp_ptr++;
        i++;
    }
}

// re-print string _readBufPtr and move cursor back
void CmdParser::rePrintPostStr(void)
{
    char *current_cur = _readBufPtr;
    for (size_t i = 0; i < strlen(_back_cmd); i++)
    {
        insertChar(_back_cmd[i]);
    }
    moveBufPtr(current_cur);
}

// cmd is a copy of the original input
//
// return the corresponding CmdExec* if "cmd" matches any command in _cmdMap
// return 0 if not found.
//
// Please note:
// ------------
// 1. The mandatory part of the command string (stored in _cmdMap) must match
// 2. The optional part can be partially omitted.
// 3. All string comparison are "case-insensitive".
//
CmdExec *CmdParser::getCmd(string cmd)
{
    CmdExec *e = 0;

    map<const string, CmdExec *>::iterator it;

    string eff_cmd;
    string end_cmd;

    // If commands haven't be registered, ignored it.
    for (it = _cmdMap.begin(); it != _cmdMap.end(); it++)
    {
        if (cmd.size() < it->first.size())
            continue;

        // Get mandatory part
        eff_cmd = cmd.substr(0, it->first.size());
        // Get optional part
        end_cmd = cmd.substr(it->first.size(), cmd.size());

        // Check mandatory part
        if (myStrNCmp(it->first, eff_cmd, it->first.size()) == 0)
        {
            // Check optional part
            if (end_cmd.size() != 0)
            {
                for (size_t i = 0; i < end_cmd.size(); i++)
                {
                    if (toupper(it->second->getOptCmd()[i]) != toupper(end_cmd[i]))
                        return 0;
                }
            }

            e = it->second;
            return e;
        }
    }
    return 0;
}

//----------------------------------------------------------------------
//    Member Function for class CmdExec
//----------------------------------------------------------------------
// return false if option contains an token
bool CmdExec::lexNoOption(const string &option) const
{
    string err;
    myStrGetTok(option, err);
    if (err.size())
    {
        errorOption(CMD_OPT_EXTRA, err);
        return false;
    }
    return true;
}

// Return false if error options found
// "optional" = true if the option is optional XD
// "optional": default = true
//
bool CmdExec::lexSingleOption(const string &option, string &token, bool optional) const
{
    size_t n = myStrGetTok(option, token);
    if (!optional)
    {
        if (token.size() == 0)
        {
            errorOption(CMD_OPT_MISSING, "");
            return false;
        }
    }
    if (n != string::npos)
    {
        errorOption(CMD_OPT_EXTRA, option.substr(n));
        return false;
    }
    return true;
}

// if nOpts is specified (!= 0), the number of tokens must be exactly = nOpts
// Otherwise, return false.
//
bool CmdExec::lexOptions(const string &option, vector<string> &tokens, size_t nOpts) const
{
    string token;
    size_t n = myStrGetTok(option, token);
    while (token.size())
    {
        tokens.push_back(token);
        n = myStrGetTok(option, token, n);
    }
    if (nOpts != 0)
    {
        if (tokens.size() < nOpts)
        {
            errorOption(CMD_OPT_MISSING, "");
            return false;
        }
        if (tokens.size() > nOpts)
        {
            errorOption(CMD_OPT_EXTRA, tokens[nOpts]);
            return false;
        }
    }
    return true;
}

CmdExecStatus
CmdExec::errorOption(CmdOptionError err, const string &opt) const
{
    switch (err)
    {
    case CMD_OPT_MISSING:
        cerr << "Error: Missing option";
        if (opt.size())
            cerr << " after (" << opt << ")";
        cerr << "!!" << endl;
        break;
    case CMD_OPT_EXTRA:
        cerr << "Error: Extra option!! (" << opt << ")" << endl;
        break;
    case CMD_OPT_ILLEGAL:
        cerr << "Error: Illegal option!! (" << opt << ")" << endl;
        break;
    case CMD_OPT_FOPEN_FAIL:
        cerr << "Error: cannot open file \"" << opt << "\"!!" << endl;
        break;
    default:
        cerr << "Error: Unknown option error type!! (" << err << ")" << endl;
        exit(-1);
    }
    return CMD_EXEC_ERROR;
}
