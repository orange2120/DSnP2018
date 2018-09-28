/****************************************************************************
  FileName     [ cmdReader.cpp ]
  PackageName  [ cmd ]
  Synopsis     [ Define command line reader member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/
#include <cassert>
#include <cstring>
#include "cmdParser.h"

using namespace std;

//----------------------------------------------------------------------
//    Extrenal funcitons
//----------------------------------------------------------------------
void mybeep();
char mygetc(istream &);
ParseChar getChar(istream &);

//----------------------------------------------------------------------
//    Member Function for class Parser
//----------------------------------------------------------------------
void CmdParser::readCmd()
{
    if (_dofile.is_open())
    {
        readCmdInt(_dofile);
        _dofile.close();
    }
    else
        readCmdInt(cin);
}

void CmdParser::readCmdInt(istream &istr)
{
    resetBufAndPrintPrompt();

    while (1)
    {
        ParseChar pch = getChar(istr);
        if (pch == INPUT_END_KEY)
            break;
        switch (pch)
        {
        case LINE_BEGIN_KEY:
        case HOME_KEY:
            moveBufPtr(_readBuf);
            break;
        case LINE_END_KEY:
        case END_KEY:
            moveBufPtr(_readBufEnd);
            break;
        case BACK_SPACE_KEY: /* TODO */
            moveBufPtr(_readBufPtr - 1);
            deleteChar();
            break;
        case DELETE_KEY:
            deleteChar();
            break;
        case NEWLINE_KEY:
            addHistory();
            cout << char(NEWLINE_KEY);
            resetBufAndPrintPrompt();
            break;
        case ARROW_UP_KEY:
            moveToHistory(_historyIdx - 1);
            break;
        case ARROW_DOWN_KEY:
            moveToHistory(_historyIdx + 1);
            break;
        case ARROW_RIGHT_KEY: /* TODO */
            moveBufPtr(_readBufPtr + 1);
            break;
        case ARROW_LEFT_KEY: /* TODO */
            moveBufPtr(_readBufPtr - 1);
            break;
        case PG_UP_KEY:
            moveToHistory(_historyIdx - PG_OFFSET);
            break;
        case PG_DOWN_KEY:
            moveToHistory(_historyIdx + PG_OFFSET);
            break;
        case TAB_KEY: /* TODO */
            break;
        case INSERT_KEY: // not yet supported; fall through to UNDEFINE
        case UNDEFINED_KEY:
            mybeep();
            break;
        default: // printable character
            insertChar(char(pch));
            break;
        }
#ifdef TA_KB_SETTING
        taTestOnly();
#endif
    }
}

// This function moves _readBufPtr to the "ptr" pointer
// It is used by left/right arrowkeys, home/end, etc.
//
// Suggested steps:
// 1. Make sure ptr is within [_readBuf, _readBufEnd].
//    If not, make a beep sound and return false. (DON'T MOVE)
// 2. Move the cursor to the left or right, depending on ptr
// 3. Update _readBufPtr accordingly. The content of the _readBuf[] will
//    not be changed
//
// [Note] This function can also be called by other member functions below
//        to move the _readBufPtr to proper position.
bool CmdParser::moveBufPtr(char *const ptr)
{
    // TODO...

    int move_length = 0;

    if ((ptr < _readBuf) || (ptr > _readBufEnd))
    {
        mybeep();
        return false;
    }

    move_length = ptr - _readBufPtr;

    if (move_length < 0)
    {
        for (int i = 0; i < abs(move_length); i++)
        {
            cout << "\b";
            _readBufPtr--;
        }
    }
    else if (move_length > 0)
    {
        for (int i = 0; i < abs(move_length); i++)
        {
            cout << *_readBufPtr; //Print tail string from _readBufPtr to _readBufEnd
            _readBufPtr++;
        }
    }

    //cout << get_tail_str(_readBuf, _readBufPtr);

    return true;
}

// [Notes]
// 1. Delete the char at _readBufPtr
// 2. mybeep() and return false if at _readBufEnd
// 3. Move the remaining string left for one character
// 4. The cursor should stay at the same position
// 5. Remember to update _readBufEnd accordingly.
// 6. Don't leave the tailing character.
// 7. Call "moveBufPtr(...)" if needed.
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteChar()---
//
// cmd> This is he command
//              ^
//
bool CmdParser::deleteChar()
{
    // TODO...
    if (strlen(_readBuf) == 0)
    {
        mybeep();
        return false;
    }
    char *tmp_ptr = _readBufPtr;

    while (tmp_ptr < _readBufEnd)
    {
        *tmp_ptr = *(tmp_ptr + 1);
        tmp_ptr++;
    }

    _readBufEnd--;
    *_readBufEnd = '\0'; //Set _readBufEnd to '\0'

    tmp_ptr = _readBufPtr;
    while (tmp_ptr <= _readBufEnd)
    {
        cout << *tmp_ptr; //Print tail string (From _readBufPtr)
        tmp_ptr++;
    }

    cout << " "; //Clear the last character

    for (int i = 0; i < (_readBufEnd - _readBufPtr + 1); i++)
    {
        cout << "\b"; //Move cursor back
    }
    return true;
}

// 1. Insert character 'ch' for "repeat" times at _readBufPtr
// 2. Move the remaining string right for "repeat" characters
// 3. The cursor should move right for "repeats" positions afterwards
// 4. Default value for "repeat" is 1. You should assert that (repeat >= 1).
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling insertChar('k', 3) ---
//
// cmd> This is kkkthe command
//                 ^
//
void CmdParser::insertChar(char ch, int repeat)
{
    // TODO...
    assert(repeat >= 1);
    for (int j = 0; j < repeat; j++)
    {
        char *tmp_ptr = _readBufEnd;

        _readBufEnd++; //Move right
        //Right shift the tail string
        while (tmp_ptr >= _readBufPtr)
        {
            *tmp_ptr = *(tmp_ptr - 1);
            tmp_ptr--;
        }

        //Move _readBufPtr and replace "\0" with ch
        _readBufPtr++;
        *(_readBufPtr - 1) = ch;

        tmp_ptr = _readBufPtr - 1; //Set tmp_ptr to _readBufPtr
                                   //if (_readBufPtr != _readBuf)
                                   //{
        while (tmp_ptr < _readBufEnd)
        {
            cout << *tmp_ptr; //Print tail string (From _readBufPtr)
            tmp_ptr++;
        }
        for (int i = 0; i < (_readBufEnd - _readBufPtr); i++)
        {
            cout << "\b"; //Move cursor back
        }
        //}
    }
}

// 1. Delete the line that is currently shown on the screen
// 2. Reset _readBufPtr and _readBufEnd to _readBuf
// 3. Make sure *_readBufEnd = 0
//
// For example,
//
// cmd> This is the command
//              ^                (^ is the cursor position)
//
// After calling deleteLine() ---
//
// cmd>
//      ^
//
void CmdParser::deleteLine()
{
    // TODO...

    _readBufPtr = _readBufEnd;

    while (_readBufPtr >= _readBuf)
    {
        *_readBufPtr = '\0';
        cout << " ";
        _readBufPtr--;
    }
    _readBufEnd = _readBuf;
    moveBufPtr(_readBuf);
}

// This functions moves _historyIdx to index and display _history[index]
// on the screen.
//
// Need to consider:
// If moving up... (i.e. index < _historyIdx)
// 1. If already at top (i.e. _historyIdx == 0), beep and do nothing.
// 2. If at bottom, temporarily record _readBuf to history.
//    (Do not remove spaces, and set _tempCmdStored to "true")
// 3. If index < 0, let index = 0.
//
// If moving down... (i.e. index > _historyIdx)
// 1. If already at bottom, beep and do nothing
// 2. If index >= _history.size(), let index = _history.size() - 1.
//
// Assign _historyIdx to index at the end.
//
// [Note] index should not = _historyIdx
// Index from 0 to _historyIdx
//
void CmdParser::moveToHistory(int index)
{
    // TODO...
    //Check boundary , index MIN=0,MAX=_history.size()
    if (index < 0 || index >= _history.size())
    {
        if (index < 0)
            index = 0;
        else if (index >= _history.size())
            index = _history.size();

        mybeep();
        return;
    }

    if (strlen(_readBuf) != 0 && _historyIdx == _history.size())
    {
        _tempCmdStored = true;
        addHistory();
    }

    retrieveHistory();

    _historyIdx = index;

    //moveBufPtr(_readBufEnd)
}

// This function adds the string in _readBuf to the _history.
// The size of _history may or may not change. Depending on whether
// there is a temp history string.
//
// 1. Remove ' ' at the beginning and end of _readBuf
// 2. If not a null string, add string to _history.
//    Be sure you are adding to the right entry of _history.
// 3. If it is a null string, don't add anything to _history.
// 4. Make sure to clean up "temp recorded string" (added earlier by up/pgUp,
//    and reset _tempCmdStored to false
// 5. Reset _historyIdx to _history.size() // for future insertion
//
void CmdParser::addHistory()
{
    // TODO...
    string temp_history;
    temp_history.assign(_readBuf); //Convert to string

    if (!temp_history.empty())
    {
        //Ignore space character before commands
        if (!_tempCmdStored)
        {
            temp_history.erase(0, temp_history.find_first_not_of(" "));
            temp_history.erase(temp_history.find_last_not_of(" ") + 1);
        }

        //Push back into _history
        _history.push_back(temp_history);
        _historyIdx = _history.size();
    }

    deleteLine();

    _tempCmdStored = false;
}

// 1. Replace current line with _history[_historyIdx] on the screen
// 2. Set _readBufPtr and _readBufEnd to end of line
//
// [Note] Do not change _history.size().
//
void CmdParser::retrieveHistory()
{
    //Get string from _history
    string temp_str;
    temp_str = _history[_historyIdx - 1];
    strcpy(_readBuf, temp_str.c_str());

    moveBufPtr(_readBuf);
    cout << _readBuf;

    _readBufPtr = _readBufEnd = _readBuf + _history[_historyIdx - 1].size();
}