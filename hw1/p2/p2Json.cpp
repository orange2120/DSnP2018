/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "p2Json.h"

using namespace std;

typedef unsigned int UINT;

#define MAX_LEN 100 //For char[] use

Json::Json()
{
} //Constructor of json object

// Implement member functions of class Row and Table here
bool Json::read(const string &jsonFile)
{
    char temp_str[MAX_LEN];
    fstream jsf;
    jsf.open(jsonFile, ios::in);
    if (!jsf.is_open())
        return false;

    while (jsf.getline(temp_str, MAX_LEN, '\n')) //Read strings line by line
    {
        char temp_key_s[MAX_LEN];
        string temp_key;
        char temp_value_s[MAX_LEN];
        int temp_value;

        //**********DEBUG**********
        //cout << temp_str << endl;
        //*************************

        if (strchr(temp_str, '{') != NULL || strspn(temp_str, " \t\n") == strlen(temp_str)) //Drop first line '{' and blank line
        {
            continue;
        }
        else if (strchr(temp_str, '}') != NULL) //End of json file ,find the terminal character '}'
        {
            break;
        }

        //strcpy(temp_str_s, temp_str.c_str());
        //String analysis
        sscanf(temp_str, " \"%[^\"]s\"", temp_key_s);
        sscanf(temp_str, "%*s%*s%[^,]s", temp_value_s);

        temp_value = atoi(temp_value_s);

        //**********DEBUG**********
        //cout << temp_key_s << "," << temp_value << endl;
        //*************************

        temp_key.assign(temp_key_s); //Convert char[] to string

        JsonElem j(temp_key, temp_value); //Constructor

        _obj.push_back(j); //Push json element into vector
    }

    jsf.close();

    return true;
}

bool Json::isEmpty(void)
{
    if (_obj.size() == 0)
        return true;
    else
        return false;
}

void Json::print(void)
{
    cout << "{" << endl;
    for (UINT i = 0; i < _obj.size(); i++)
    {
        cout << "  " << _obj[i];
        if (i != _obj.size() - 1)
            cout << ",";
        cout << endl;
    }
    cout << "}" << endl;
}

bool Json::add(string &str)
{
    char str_s[MAX_LEN];
    char temp_key_s[MAX_LEN];
    string temp_key;
    int temp_value;

    strcpy(str_s, str.c_str());

    sscanf(str_s, "%*s %s %d", temp_key_s, &temp_value);

    //**********DEBUG**********
    cout << "key=" << temp_key_s << ",value=" << temp_value << endl;
    //*************************

    if (temp_key_s == NULL)
        return false;

    temp_key.assign(temp_key_s);

    JsonElem j(temp_key, temp_value); //Construct a json element object

    _obj.push_back(j); //Push json element into vector

    return true;
}

int Json::get_sum(void)
{
    int sum = 0;
    for (UINT i = 0; i < _obj.size(); i++)
    {
        sum += _obj[i].get_value();
    }

    return sum;
}

float Json::get_ave(void)
{
    return (float)Json::get_sum() / _obj.size();
}

int Json::get_max(void)
{
    int max = 0;
    for (UINT i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].get_value() > max)
            max = _obj[i].get_value();
    }
    return max;
}

int Json::get_min(void)
{
    int min = _obj[0].get_value();
    for (UINT i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].get_value() < min)
            min = _obj[i].get_value();
    }
    return min;
}

JsonElem Json::get_max_element(void)
{
    int max = 0;
    int max_index = 0;
    for (UINT i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].get_value() > max)
        {
            max = _obj[i].get_value();
            max_index = i;
        }
    }
    return _obj[max_index];
}

JsonElem Json::get_min_element(void)
{
    int min = _obj[0].get_value();
    int min_index = 0;
    for (UINT i = 0; i < _obj.size(); i++)
    {
        if (_obj[i].get_value() < min)
        {
            min = _obj[i].get_value();
            min_index = i;
        }
    }
    return _obj[min_index];
}

ostream &
operator<<(ostream &os, const JsonElem &j)
{
    return (os << "\"" << j._key << "\" : " << j._value);
}

string JsonElem::get_key(void)
{
    return _key;
}

int JsonElem::get_value(void)
{
    return _value;
}