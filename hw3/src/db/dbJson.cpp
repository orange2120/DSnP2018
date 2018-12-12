/****************************************************************************
  FileName     [ DBJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstring>
#include "dbJson.h"
#include "util.h"

using namespace std;

#define MAX_LEN 1024 //For char[] use

/*****************************************/
/*          Global Functions             */
/*****************************************/

ostream &
operator<<(ostream &os, const DBJsonElem &j)
{
    return (os << "\"" << j._key << "\" : " << j._value);
}

istream &
operator>>(istream &is, DBJson &j)
{
    // TODO: to read in data from Json file and store them in a DB
    // - You can assume the input file is with correct JSON file format
    // - NO NEED to handle error file format
    assert(j._obj.empty());
    char temp_str[MAX_LEN];

    while (is.getline(temp_str, MAX_LEN, '\n')) //Read strings line by line
    {
        char temp_key_s[MAX_LEN];
        string temp_key;
        char temp_value_s[MAX_LEN];
        int temp_value;

        if (strchr(temp_str, '{') != NULL || strspn(temp_str, " \t\n") == strlen(temp_str)) //Drop first line '{' and blank line
        {
            continue;
        }
        else if (strchr(temp_str, '}') != NULL) //End of json file ,find the terminal character '}'
        {
            break;
        }

        //String analysis
        sscanf(temp_str, " \"%[^\"]s\"", temp_key_s);
        sscanf(temp_str, "%*s%*s%[^,]s", temp_value_s);

        temp_value = atoi(temp_value_s);

        temp_key.assign(temp_key_s); //Convert char[] to string

        DBJsonElem jse(temp_key, temp_value); //Constructor

        j.add(jse);
    }
    j._json_read = true;
    return is;
}

ostream &
operator<<(ostream &os, const DBJson &j)
{
    os << "{" << endl;
    for (size_t i = 0; i < j.size(); i++)
    {
        os << "  " << j[i];
        if (i < j.size() - 1)
        {
            os << ",";
        }
        os << endl;
    }
    os << "}" << endl;
    return os;
}

/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
// Implement member functions of class Row and Table here
// return false if key is repeated
bool DBJson::add(const DBJsonElem &elm)
{
    if (!isKeyExist(elm.key()))
    {
        //Push JSON element into vector
        _obj.push_back(elm);
        return true;
    }
    return false;
}

// Delete element from key
bool DBJson::del(const string &key)
{
    if (_obj.empty())
        return false;

    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].key() == key)
        {
            _obj.erase(_obj.begin() + i);
            return true;
        }
    }
    return false;
}

// return NAN if DBJson is empty
float DBJson::ave(void) const
{
    if (_obj.empty())
        return NAN;
    else
        return (float)DBJson::sum() / _obj.size();
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::max(size_t &idx) const
{
    int maxN = INT_MIN;

    if (_obj.empty())
    {
        idx = _obj.size();
        return INT_MIN;
    }

    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].value() > maxN)
        {
            maxN = _obj[i].value();
            idx = i;
        }
    }
    return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int DBJson::min(size_t &idx) const
{
    int minN = INT_MAX;

    if (_obj.empty())
    {
        idx = _obj.size();
        return INT_MIN;
    }

    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].value() < minN)
        {
            minN = _obj[i].value();
            idx = i;
        }
    }
    return minN;
}

void DBJson::sort(const DBSortKey &s)
{
    // Sort the data according to the order of columns in 's'
    ::sort(_obj.begin(), _obj.end(), s);
}

void DBJson::sort(const DBSortValue &s)
{
    // Sort the data according to the order of columns in 's'
    ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int DBJson::sum(void) const
{
    int s = 0;
    for (size_t i = 0; i < _obj.size(); i++)
    {
        s += _obj[i].value();
    }
    return s;
}

/**
 * Get element index from a key.
 * Usage:get_key_idx(key, index)
 * Return false if there is no matched key 
 */
bool DBJson::key_idx(const string &key, size_t &idx) const
{
    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].key() == key)
        {
            idx = i;
            return true;
        }
    }
    return false;
}

// To check if the key has already exist.
bool DBJson::isKeyExist(const string &key)
{
    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].key() == key)
            return true;
    }
    return false;
}

void DBJson::reset(void)
{
    _obj.clear();
    _json_read = false;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
