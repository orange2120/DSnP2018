/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem
{
  public:
    // TODO: define constructor & member functions on your own
    JsonElem() {}
    JsonElem(const string &k, int v) : _key(k), _value(v) {}

    string get_key(void);
    int get_value(void);

    friend ostream &operator<<(ostream &, const JsonElem &);

  private:
    string _key; // DO NOT change this definition. Use it to store key.
    int _value;  // DO NOT change this definition. Use it to store value.
};

class Json
{
  public:
    // TODO: define constructor & member functions on your own
    Json();
    bool read(const string &);
    bool write_to_file(const string &);
    bool isEmpty(void);
    void print(void);

    bool add(string &);
    int get_sum(void);
    float get_ave(void);
    int get_max(void);
    int get_min(void);
    JsonElem get_max_element(void);
    JsonElem get_min_element(void);

  private:
    vector<JsonElem> _obj; // DO NOT change this definition.
                           // Use it to store JSON elements.
};

#endif // P2_TABLE_H
