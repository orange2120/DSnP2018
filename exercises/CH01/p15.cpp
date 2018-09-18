#include <cstdio>
#include <iostream>
using namespace std;

int main()
{
    int a = 10;
    int &r = a;
    int b = 20;

    cout << "a=" << a << endl;
    cout << "r=" << r << endl;
    r = b;
    cout << "b=" << b << endl;
    cout << "r=" << r << endl;
}