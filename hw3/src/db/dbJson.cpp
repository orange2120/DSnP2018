/****************************************************************************
  FileName     [ DBJson.cpp ]
  PackageName  [ DBJson ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
               [ Modified by Orange Hsu ]
****************************************************************************/
#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "dbJson.h"

using namespace std;

#define MAX_LEN 100 //For char[] use

/**
 * 
 * DBJson class
 *  
 */
/*
// Implement member functions of class Row and Table here

bool DBJson::write_to_file(const string &jsonFile)
{
    fstream jsf;
    jsf.open(jsonFile, ios::in | ios::out | ios::trunc);
    if (!jsf.is_open())
        return false;

    //File start
    jsf << "{" << endl;
    for (UINT i = 0; i < _obj.size(); i++)
    {
        jsf << "  " << _obj[i];
        if (i != _obj.size() - 1)
            jsf << ",";
        jsf << endl;
    }
    jsf << "}" << endl;

    jsf.close();
    return true;
}
*/
bool DBJson::add(const DBJsonElem &jse)
{
    if (!isKeyExist(jse.key()))
    {
        _obj.push_back(jse); //Push json element into vector
        return true;
    }
    return false;
}

/**
 * Delete element from key
 */
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

/**
 * Get index of a key.
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

/**
 * To check if the key has already exist.
 */
bool DBJson::isKeyExist(const string &key)
{
    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].key() == key)
            return true;
    }
    return false;
}

int DBJson::sum(void) const
{
    int sum = 0;
    for (size_t i = 0; i < _obj.size(); i++)
    {
        sum += _obj[i].value();
    }

    return sum;
}

float DBJson::ave(void) const
{
    return (float)DBJson::sum() / _obj.size();
}

int DBJson::max(size_t &idx) const
{
    int max = 0;
    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].value() > max)
        {
            max = _obj[i].value();
            idx = i;
        }
    }
    return max;
}

int DBJson::min(size_t &idx) const
{
    int min = _obj[0].value();
    for (size_t i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].value() < min)
        {
            min = _obj[i].value();
            idx = i;
        }
    }
    return min;
}

void DBJson::sort(const DBSortKey &dsk)
{
    //DBSortKey()
}

void DBJson::sort(const DBSortValue &)
{
}

void DBJson::reset(void)
{
    _obj.clear();
    _json_read = false;
}

istream &
operator>>(istream &is, DBJson &j)
{
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

        //_obj.push_back(dbj); //Push json element into vector
        j.add(jse);
    }
    j._json_read = true;
    return is;
}

ostream &
operator<<(ostream &os, const DBJson &j)
{
    os << "{";
    for (size_t i = 0; i < j.size(); i++)
    {
        os << "  " << j[i];
        if (i != j.size() - 1)
            os << ",";
        os << endl;
    }
    os << "}" << endl;
    return os;
}
/**
 * DBJsonElem class
 */

ostream &
operator<<(ostream &os, const DBJsonElem &j)
{
    return (os << "\"" << j._key << "\" : " << j._value);
}
