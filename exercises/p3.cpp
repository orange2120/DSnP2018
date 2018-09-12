#include <cstdio>
#include <iostream>
using namespace std;

int main()
{
    int a;
    int *p;

    cout << "a   =" << a << endl;
    cout << "ptr =" << p << endl;
    p = &a;
    cout << "&a  =" << &a << endl;
    cout << "ptr =" << p << endl;
    cout << "*ptr=" << *p << endl;
}